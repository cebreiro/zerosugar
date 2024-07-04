#include "bot_controller.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_interface.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/socket.h"
#include "zerosugar/xr/data/game_data_provider.h"
#include "zerosugar/xr/data/provider/behavior_tree_xml_provider.h"
#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/network/model/generated/lobby_sc_message.h"
#include "zerosugar/xr/network/model/generated/login_sc_message.h"

namespace zerosugar::xr
{
    BotController::BotController(const ServiceLocator& locator, SharedPtrNotNull<execution::AsioStrand> strand,
        int64_t id, const bt::NodeSerializer& nodeSerializer, std::string defaultBehaviorTree)
        : _serviceLocator(locator)
        , _strand(std::move(strand))
        , _id(id)
        , _nodeSerializer(nodeSerializer)
        , _defaultBehaviorTree(std::move(defaultBehaviorTree))
        , _socket(std::make_shared<Socket>(_strand))
        , _blackBoard(std::make_unique<bt::BlackBoard>())
    {
        _blackBoard->Insert<BotController*>("owner", this);
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

    auto BotController::Transition(const std::string& behaviorTreeName) -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        [[maybe_unused]]
        auto self = shared_from_this();

        const GameDataProvider& gameDataProvider = _serviceLocator.Get<GameDataProvider>();
        const BehaviorTreeXMLProvider& behaviorTreeProvider = gameDataProvider.GetBehaviorTreeXMLDataProvider();

        const bt::INodeDataSet* dataSet = behaviorTreeProvider.Find(behaviorTreeName);
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

        _behaviorTree = std::move(newBehaviorTree);
        _runAI = RunAI();
    }

    void BotController::InvokeOnBehaviorTree(const std::function<void(BehaviorTree&)>& func)
    {
        assert(func);

        func(*_behaviorTree);
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

        _socket->CloseAsync();

        co_await _runIOFuture;

        co_return;
    }

    void BotController::Send(Buffer buffer)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        _socket->SendAsync(std::move(buffer));
    }

    auto BotController::GetSocket() -> Socket&
    {
        return *_socket;
    }

    auto BotController::GetSessionState() const -> BotSessionStateType
    {
        return _sessionState;
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

        Buffer receiveBuffer;
        receiveBuffer.Add(buffer::Fragment::Create(1024));

        Buffer receivedBuffer;

        while (_socket->IsOpen())
        {
            const std::expected<int64_t, IOError> receiveResult = co_await _socket->ReceiveAsync(receiveBuffer);

            if (receiveResult.has_value())
            {
                const int64_t receiveSize = receiveResult.value();
                assert(receiveSize <= receiveBuffer.GetSize());

                receivedBuffer.MergeBack([&]() -> Buffer
                    {
                        Buffer temp;

                        [[maybe_unused]] bool result = receiveBuffer.SliceFront(temp, receiveSize);
                        assert(result);

                        return temp;
                    }());

                while (true)
                {
                    if (receivedBuffer.GetSize() < 2)
                    {
                        break;
                    }

                    PacketReader reader(receivedBuffer.cbegin(), receivedBuffer.cend());

                    const int32_t packetSize = reader.Read<int16_t>();
                    if (receivedBuffer.GetSize() < packetSize)
                    {
                        if (receiveBuffer.GetSize() < packetSize - 2)
                        {
                            receiveBuffer.Add(buffer::Fragment::Create(packetSize));
                        }

                        break;
                    }

                    std::any packet;

                    switch (_sessionState)
                    {
                    case BotSessionStateType::Login:
                    {
                        packet = network::login::sc::CreateAnyFrom(reader);
                        assert(reader.GetReadSize() == packetSize);
                    }
                    break;
                    case BotSessionStateType::Lobby:
                    {
                        packet = network::lobby::sc::CreateAnyFrom(reader);
                        assert(reader.GetReadSize() == packetSize);
                    }
                    break;
                    case BotSessionStateType::Game:
                    {
                        packet = network::game::sc::CreateAnyFrom(reader);
                        assert(reader.GetReadSize() == packetSize);
                    }
                    break;
                    default:
                        assert(false);
                    }

                    Buffer temp;
                    [[maybe_unused]] bool sliced = receivedBuffer.SliceFront(temp, packetSize);
                    assert(sliced);

                    if (_behaviorTree->IsWaitFor(packet.type()))
                    {
                        _behaviorTree->Notify(packet);
                    }
                }
            }
            else
            {
                ZEROSUGAR_LOG_INFO(_serviceLocator,
                    std::format("[bot_controller] receive error. error: {}", receiveResult.error().message));

                co_return;
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

            ZEROSUGAR_LOG_DEBUG(_serviceLocator,
                std::format("[{}] behavior_tree[{}] tick", GetName(), bt->GetName()));

            bt->RunOnce();

            while (bt->IsAwaiting())
            {
                completionToken->Reset();

                bt->SetSignalHandler([completionToken]()
                    {
                        completionToken->OnSuccess();
                    });

                co_await Future<void>(completionToken);

                ZEROSUGAR_LOG_DEBUG(_serviceLocator,
                    std::format("[{}] behavior_tree[{}] resume", GetName(), bt->GetName()));

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
            std::format("[bot_controller] shutdown. reason: {}", reason));

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
        return std::format("bot_controller:{}", _id);
    }

    auto BotController::GetStrand() const -> execution::AsioStrand&
    {
        return *_strand;
    }
}
