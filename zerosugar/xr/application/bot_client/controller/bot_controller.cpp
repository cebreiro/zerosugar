#include "bot_controller.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_interface.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/socket.h"
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/network/model/generated/lobby_sc_message.h"
#include "zerosugar/xr/network/model/generated/login_sc_message.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/event/connect_event.h"
#include "zerosugar/xr/data/behavior_tree_xml_provider.h"

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
        _blackBoard->Insert<std::pair<std::string, int32_t>>("login", { "127.0.0.1", 8181 });
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

        const bt::INodeDataSet* dataSet = _serviceLocator.Get<BehaviorTreeXMLProvider>().Find(behaviorTreeName);
        if (!dataSet)
        {
            assert(false);

            co_return;
        }

        auto newBehaviorTree = std::make_shared<BehaviorTree>(*_blackBoard);
        newBehaviorTree->Initialize(dataSet->Deserialize(_nodeSerializer));

        if (_behaviorTree)
        {
            _behaviorTree->RequestStop();
        }

        if (_runAI.IsValid())
        {
            co_await _runAI;
        }

        _behaviorTree = std::move(newBehaviorTree);
        _runAI = RunAI();
    }

    auto BotController::ConnectTo(std::string ip, uint16_t port, int32_t retryMilli) -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(*_strand));
        assert(!_socket->IsOpen());

        [[maybe_unused]]
        auto self = shared_from_this();

        co_await _socket->ConnectAsync(ip, port, std::chrono::milliseconds(retryMilli));
        _runIOFuture = RunIO();

        Post(*_strand, [self = shared_from_this()]()
            {
                self->_behaviorTree->Notify(event::OnSessionConnected{});
            });

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

        constexpr auto minTickInternal = std::chrono::milliseconds(200);
        constexpr auto maxTickInternal = std::chrono::milliseconds(1000);

        std::chrono::milliseconds tickInterval = minTickInternal;

        SharedPtrNotNull<BehaviorTree> bt = _behaviorTree;
        bt->RunOnce();

        while (true)
        {
            co_await Delay(tickInterval);
            assert(ExecutionContext::IsEqualTo(*_strand));

            if (bt->StopRequested())
            {
                bt->Finalize();

                co_return;
            }

            if (bt->IsAwaiting())
            {
                tickInterval = std::min(tickInterval + std::chrono::milliseconds(100), maxTickInternal);
            }
            else
            {
                tickInterval = minTickInternal;

                bt->RunOnce();
            }
        }
    }

    void BotController::Shutdown(const std::string& reason)
    {
        (void)reason;
    }

    auto BotController::GetId() const -> int64_t
    {
        return _id;
    }
}
