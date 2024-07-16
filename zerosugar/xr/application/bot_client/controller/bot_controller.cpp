#include "bot_controller.h"

#include <boost/scope/scope_exit.hpp>
#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_interface.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/execution/executor/operation/schedule.h"
#include "zerosugar/shared/network/socket.h"
#include "zerosugar/xr/application/bot_client/controller/bot_shared_context.h"
#include "zerosugar/xr/application/bot_client/controller/ai/movement/bot_movement_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/local_player.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/visual_object_container.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/monster.h"
#include "zerosugar/xr/data/game_data_provider.h"
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/navigation/navi_data_provider.h"
#include "zerosugar/xr/network/packet.h"
#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/network/model/generated/lobby_sc_message.h"
#include "zerosugar/xr/network/model/generated/login_sc_message.h"

namespace zerosugar::xr
{
    BotController::BotController(const ServiceLocator& locator, SharedPtrNotNull<Strand> strand, bot::SharedContext& sharedContext,
        SharedPtrNotNull<Socket> socket, int64_t id, const bt::NodeSerializer& nodeSerializer, std::string defaultBehaviorTree)
        : _serviceLocator(locator)
        , _strand(std::move(strand))
        , _sharedContext(sharedContext)
        , _id(id)
        , _nodeSerializer(nodeSerializer)
        , _defaultBehaviorTree(std::move(defaultBehaviorTree))
        , _socket(std::move(socket))
        , _randomEngine(std::random_device{}())
        , _blackBoard(std::make_unique<bt::BlackBoard>())
        , _visualObjectContainer(std::make_unique<bot::VisualObjectContainer>())
        , _movementController(std::make_unique<bot::MovementController>(*this))
    {
        _blackBoard->Insert<BotController*>(name, this);
        _blackBoard->Insert<std::pair<std::string, int32_t>>("login_address", { "127.0.0.1", 8181 });
    }

    BotController::~BotController()
    {
    }

    void BotController::Start()
    {
        Post(*_strand, [self = shared_from_this()]()
            {
                self->Transition(self->_defaultBehaviorTree);
            });
    }

    void BotController::StartDebugOutputString()
    {
        constexpr auto delay = std::chrono::seconds(15);

        Schedule(*_strand, [weak = weak_from_this()]()
            {
                auto self = weak.lock();
                if (!self)
                {
                    return;
                }

                ZEROSUGAR_LOG_INFO(self->GetServiceLocator(),
                    fmt::format("[{}] socket: {}, state: {}, behavior_tree: {}, awaiting: [{}, event: {}], current_node: {}, receive_buffer: {}, received_buffer: {}, strand task: {}",
                        self->GetName(),
                        self->GetSocket().IsOpen() ? "open" : "closed",
                        ToString(self->GetSessionState()),
                        self->_behaviorTree ? self->_behaviorTree->GetName() : "null",
                        self->_behaviorTree ? (self->_behaviorTree->IsAwaiting() ? "true" : "false") : "null",
                        self->_behaviorTree ? self->_behaviorTree->GetCurrentAwaitEventName().value_or("null") : "null",
                        self->_behaviorTree ? self->_behaviorTree->GetCurrentVisitNodeName().value_or("null") : "null",
                        self->_receiveBuffer.GetSize(),
                        self->_packetBuffer.GetSize(),
                        self->_strand->GetTaskCount()));

            }, delay, delay);
    }

    auto BotController::Transition(std::string behaviorTreeName) -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        [[maybe_unused]]
        auto self = shared_from_this();

        const GameDataProvider& gameDataProvider = _serviceLocator.Get<GameDataProvider>();

        const bt::INodeDataSet* dataSet = gameDataProvider.FindBehaviorTree(behaviorTreeName);
        if (!dataSet)
        {
            assert(false);

            co_return;
        }

        auto newBehaviorTree = std::make_shared<BehaviorTree>(*_blackBoard);
        newBehaviorTree->Initialize(behaviorTreeName, dataSet->Deserialize(_nodeSerializer));
        newBehaviorTree->SetLogger(_behaviorTreeLogger);

        if (_behaviorTree)
        {
            _behaviorTree->RequestStop();
            _behaviorTree.reset();
        }

        if (_runAI.IsValid())
        {
            co_await _runAI;
        }

        ZEROSUGAR_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] behavior transition: {}",
                GetName(), behaviorTreeName));

        _behaviorTree = std::move(newBehaviorTree);
        _runAI = RunAI();
    }

    void BotController::InvokeOnBehaviorTree(const std::function<void(BehaviorTree&)>& func)
    {
        assert(func);

        func(*_behaviorTree);
    }

    auto BotController::Ping(int64_t sequence) -> Future<std::optional<std::chrono::system_clock::duration>>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;

        if (_sessionState != BotSessionStateType::Game || !_localPlayer)
        {
            co_return std::nullopt;
        }

        assert(!_pingFutures.contains(sequence));

        Promise<std::chrono::system_clock::duration>& promise = _pingPromises[sequence];
        auto future = promise.GetFuture();

        network::game::cs::Ping ping;
        ping.sequence = sequence;
        ping.clientTimePoint = std::chrono::system_clock::now().time_since_epoch().count();

        SendToServer(Packet::ToBuffer(ping));

        const auto result = co_await future;
        _pingPromises.erase(sequence);

        co_return result;
    }

    auto BotController::ConnectTo(std::string ip, uint16_t port, int32_t retryMilli) -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(*_strand));
        assert(!_socket->IsOpen());

        [[maybe_unused]]
        auto self = shared_from_this();

        co_await _socket->ConnectAsync(ip, port, std::chrono::milliseconds(retryMilli));
        _runIOFuture = RunIO();

        co_return;
    }

    auto BotController::Close() -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(*_strand));
        assert(_runIOFuture.IsValid());

        [[maybe_unused]]
        auto self = shared_from_this();

        // post
        _socket->CloseAsync();

        if (_localPlayer)
        {
            TryRemoveNavigation(_localPlayer->GetZoneId());
        }

        // end
        co_await _runIOFuture;

        co_return;
    }

    void BotController::SendToServer(Buffer buffer)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        _socket->SendAsync(std::move(buffer)).Then(*_strand,
            [weak = weak_from_this()](const std::expected<int64_t, IOError>& expected)
            {
                if (expected.has_value())
                {
                    return;
                }

                const std::shared_ptr<BotController>& self = weak.lock();
                if (!self)
                {
                    return;
                }

                ZEROSUGAR_LOG_ERROR(self->GetServiceLocator(),
                    fmt::format("[{}] send error. error: {}",
                        self->GetName(), expected.error().message));
            });
    }

    auto BotController::GetSocket() -> Socket&
    {
        return *_socket;
    }

    auto BotController::GetRandomEngine() -> std::mt19937&
    {
        return _randomEngine;
    }

    auto BotController::GetSessionState() const -> BotSessionStateType
    {
        return _sessionState;
    }

    auto BotController::GetVisualObjectContainer() -> bot::VisualObjectContainer&
    {
        return *_visualObjectContainer;
    }

    auto BotController::GetVisualObjectContainer() const -> const bot::VisualObjectContainer&
    {
        return *_visualObjectContainer;
    }

    auto BotController::GetMovementController() -> bot::MovementController&
    {
        return *_movementController;
    }

    auto BotController::GetMovementController() const -> const bot::MovementController&
    {
        return *_movementController;
    }

    auto BotController::GetNavigation() -> NavigationService*
    {
        const int32_t zoneId = GetLocalPlayer().GetZoneId();

        const auto iter = _sharedContext.navigationServices.find(zoneId);

        return iter != _sharedContext.navigationServices.end() ? iter->second.second.get() : nullptr;
    }

    void BotController::SetLogger(IBehaviorTreeLogger* logger)
    {
        _behaviorTreeLogger = logger;
    }

    void BotController::SetSessionState(BotSessionStateType sessionState)
    {
        _sessionState = sessionState;
    }

    auto BotController::RunIO() -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(*_strand));
        assert(_socket->IsOpen());

        [[maybe_unused]]
        auto self = shared_from_this();

        _receiveBuffer.Clear();
        _packetBuffer.Clear();

        while (_socket->IsOpen())
        {
            if (_receiveBuffer.GetSize() < 4)
            {
                _receiveBuffer.Add(buffer::Fragment::Create(4096));
            }

            const std::expected<int64_t, IOError> receiveResult = co_await _socket->ReceiveAsync(_receiveBuffer);
            assert(ExecutionContext::IsEqualTo(*_strand));

            if (!receiveResult.has_value())
            {
                ZEROSUGAR_LOG_INFO(_serviceLocator,
                    fmt::format("[bot_controller] receive error. error: {}", receiveResult.error().message));

                co_return;
            }
            const int64_t receiveSize = receiveResult.value();
            assert(receiveSize <= _receiveBuffer.GetSize());

            _packetBuffer.MergeBack([&]() -> Buffer
                {
                    Buffer temp;

                    [[maybe_unused]]
                    const bool result = _receiveBuffer.SliceFront(temp, receiveSize);
                    assert(result);

                    return temp;
                }());

            while (_packetBuffer.GetSize() >= 4)
            {
                PacketReader reader(_packetBuffer.cbegin(), _packetBuffer.cend());
                const int32_t packetSize = reader.Read<int32_t>();

                if (_packetBuffer.GetSize() < packetSize)
                {
                    if (_receiveBuffer.GetSize() < packetSize)
                    {
                        _receiveBuffer.Add(buffer::Fragment::Create(4096 + packetSize));
                    }

                    break;
                }

                switch (_sessionState)
                {
                case BotSessionStateType::Login:
                {
                    using namespace network::login;

                    std::unique_ptr<IPacket> packet = sc::CreateFrom(reader);
                    assert(reader.GetReadSize() == packetSize);
                    assert(packet);

                    sc::Visit(*packet, [this]<typename T>(const T & packet)
                    {
                        if (_behaviorTree->IsWaitFor<T>())
                        {
                            _behaviorTree->Notify(packet);
                        }
                    });
                }
                break;
                case BotSessionStateType::Lobby:
                {
                    using namespace network::lobby;

                    std::unique_ptr<IPacket> packet = sc::CreateFrom(reader);
                    assert(reader.GetReadSize() == packetSize);
                    assert(packet);

                    sc::Visit(*packet, [this]<typename T>(const T & packet)
                    {
                        if (_behaviorTree->IsWaitFor<T>())
                        {
                            _behaviorTree->Notify(packet);
                        }
                    });
                }
                break;
                case BotSessionStateType::Game:
                {
                    using namespace network;

                    std::unique_ptr<IPacket> packet = game::sc::CreateFrom(reader);
                    assert(reader.GetReadSize() == packetSize);
                    assert(packet);

                    game::sc::Visit(*packet, [this]<typename T>(const T & packet)
                    {
                        if constexpr (std::is_same_v<T, game::sc::Pong>)
                        {
                            const auto iter = _pingPromises.find(packet.sequence);
                            assert(iter != _pingPromises.end());

                            const auto now = std::chrono::system_clock::now();
                            const auto prev = std::chrono::system_clock::time_point(std::chrono::system_clock::duration(packet.clientTimePoint));

                            iter->second.Set(now - prev);

                            _pingPromises.erase(iter);
                        }

                        if constexpr (std::is_same_v<T, game::sc::EnterGame>)
                        {
                            _localPlayer = std::make_unique<bot::LocalPlayer>(packet.localPlayer, packet.zoneId);
                            _visualObjectContainer->Clear();

                            this->TryCreateNavigation(packet.zoneId);
                        }
                        else if constexpr (std::is_same_v<T, game::sc::SpawnMonster> || std::is_same_v<T, game::sc::AddMonster>)
                        {
                            for (const game::Monster& monster : packet.monsters)
                            {
                                [[maybe_unused]]
                                const bool added = _visualObjectContainer->Add(monster.id, std::make_shared<bot::Monster>(monster));
                                assert(added);
                            }
                        }
                        else if constexpr (std::is_same_v<T, game::sc::RemoveMonster>)
                        {
                            for (const int64_t id : packet.monsters)
                            {
                                [[maybe_unused]]
                                const bool removed = _visualObjectContainer->Remove(id);
                                assert(removed);
                            }
                        }
                        else if constexpr (std::is_same_v<T, game::sc::DespawnMonster>)
                        {
                            for (const int64_t id : packet.monsters)
                            {
                                [[maybe_unused]]
                                const bool removed = _visualObjectContainer->Remove(id);
                                assert(removed);
                            }
                        }
                        else if constexpr (std::is_same_v<T, game::sc::MoveMonster>)
                        {
                            bot::Monster* monster = _visualObjectContainer->FindMonster(packet.id);
                            assert(monster);

                            const Eigen::Vector3d pos(packet.position.x, packet.position.y, packet.position.z);
                            const Eigen::Vector3d rot(packet.rotation.pitch, packet.rotation.yaw, packet.rotation.roll);

                            monster->SetPosition(pos);
                            monster->SetRotation(rot);
                        }
                        else if constexpr (std::is_same_v<T, game::sc::AttackMonster>)
                        {
                            bot::Monster* monster = _visualObjectContainer->FindMonster(packet.id);
                            assert(monster);

                            const Eigen::Vector3d pos(packet.position.x, packet.position.y, packet.position.z);
                            const Eigen::Vector3d rot(packet.rotation.pitch, packet.rotation.yaw, packet.rotation.roll);

                            monster->SetPosition(pos);
                            monster->SetRotation(rot);
                        }

                        if (_behaviorTree && _behaviorTree->IsWaitFor<T>())
                        {
                            _behaviorTree->Notify(packet);
                        }
                    });
                }
                break;
                }

                Buffer temp;
                [[maybe_unused]] bool sliced = _packetBuffer.SliceFront(temp, packetSize);
                assert(sliced);
            }
        }
    }

    auto BotController::RunAI() -> Future<void>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        const SharedPtrNotNull<BehaviorTree> bt = _behaviorTree;
        auto completionToken = std::make_shared<future::SharedContext<void>>();

        while (true)
        {
            constexpr auto tickInterval = std::chrono::milliseconds(200);
            co_await Delay(tickInterval);

            assert(ExecutionContext::IsEqualTo(*_strand));

            bt->RunOnce();

            while (bt->IsAwaiting())
            {
                completionToken->Reset();

                bt->SetOrExecuteSignalHandler([completionToken]()
                    {
                        completionToken->OnSuccess();
                    });

                co_await Future<void>(completionToken);

                if (bt->StopRequested())
                {
                    break;
                }

                bt->Resume();
            }

            if (bt->StopRequested())
            {
                break;
            }
        }

        bt->Finalize();

        co_return;
    }

    void BotController::Shutdown(const std::string& reason)
    {
        ZEROSUGAR_LOG_INFO(_serviceLocator,
            fmt::format("[bot_controller] shutdown. reason: {}", reason));

        if (_behaviorTree)
        {
            _behaviorTree->RequestStop();
            _behaviorTree.reset();
        }

        if (_socket)
        {
            _socket->CloseAsync();
        }
    }

    auto BotController::GetId() const -> int64_t
    {
        return _id;
    }

    auto BotController::GetName() const -> std::string
    {
        return fmt::format("bot_controller_{}", _id);
    }

    auto BotController::GetStrand() const -> Strand&
    {
        return *_strand;
    }

    auto BotController::GetServiceLocator() -> ServiceLocator&
    {
        return _serviceLocator;
    }

    auto BotController::GetLocalPlayer() -> bot::LocalPlayer&
    {
        assert(_localPlayer);

        return *_localPlayer;
    }

    auto BotController::GetLocalPlayer() const -> const bot::LocalPlayer&
    {
        assert(_localPlayer);

        return *_localPlayer;
    }

    void BotController::TryCreateNavigation(int32_t mapId)
    {
        const auto iter = _sharedContext.navigationServices.find(mapId);
        if (iter != _sharedContext.navigationServices.end())
        {
            ++iter->second.first;

            return;
        }

        const NavigationDataProvider& navigationDataProvider = _serviceLocator.Get<NavigationDataProvider>();
        if (!navigationDataProvider.Contains(mapId))
        {
            return;
        }

        auto naviData = navigationDataProvider.Create(mapId);
        auto navigationService = std::make_shared<NavigationService>(_serviceLocator, _sharedContext.naviStrand, std::move(naviData));

        (void)_sharedContext.navigationServices.try_emplace(mapId, 1, std::move(navigationService));
    }

    void BotController::TryRemoveNavigation(int32_t mapId)
    {
        const auto iter = _sharedContext.navigationServices.find(mapId);
        if (iter != _sharedContext.navigationServices.end())
        {
            --iter->second.first;

            if (iter->second.first == 0)
            {
                _sharedContext.navigationServices.erase(iter);
            }
        }
    }
}
