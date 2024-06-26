#include "game_server.h"
#include "zerosugar/xr/server/game/game_server.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/shared/process/query_sys_info.h"
#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/client/game_client.h"
#include "zerosugar/xr/server/game/command/command_channel_runner.h"
#include "zerosugar/xr/server/game/command/command_handler_factory.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/game_instance_container.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_serializer.h"
#include "zerosugar/xr/server/game/packet/packet_handler_factory.h"
#include "zerosugar/xr/server/game/packet/packet_handler_interface.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"

namespace zerosugar::xr
{
    GameServer::GameServer(execution::AsioExecutor& executor)
        : Server("game_server", executor)
        , _gameInstanceContainer(std::make_unique<GameInstanceContainer>())
        , _gamePacketHandlerFactory(std::make_unique<GamePacketHandlerFactory>())
        , _commandHandlerFactory(std::make_unique<CommandHandlerFactory>())
        , _gameEntitySerializer(std::make_unique<GameEntitySerializer>())
    {
    }

    GameServer::~GameServer()
    {
    }

    void GameServer::Initialize(ServiceLocator& serviceLocator)
    {
        Server::Initialize(serviceLocator);

        _serviceLocator = serviceLocator;
    }

    void GameServer::StartUp(uint16_t listenPort)
    {
        Server::StartUp(listenPort);

        _port = listenPort;

        RegisterToCoordinationService();
        OpenCoordinationCommandChannel();
        //ScheduleServerStatusReport();
    }

    void GameServer::Shutdown()
    {
        Server::Shutdown();

        if (_serverStatusReportStopSource.stop_possible())
        {
            _serverStatusReportStopSource.request_stop();
        }
    }

    bool GameServer::HasClient(session::id_type id) const
    {
        return _clients.count(id) > 0;
    }

    bool GameServer::AddClient(session::id_type id, SharedPtrNotNull<GameClient> client)
    {
        decltype(_clients)::accessor accessor;
        if (_clients.insert(accessor, id))
        {
            accessor->second = std::move(client);

            return true;
        }
        else
        {
            assert(false);

            return false;
        }
    }

    void GameServer::SendCommandResponse(const service::CoordinationCommandResponse& response)
    {
        _responseChannel->Send(response, channel::ChannelSignal::NotifyOne);
    }

    auto GameServer::GetServerId() const -> int64_t
    {
        return _serverId;
    }

    auto GameServer::GetServiceLocator() -> ServiceLocator&
    {
        return _serviceLocator;
    }

    auto GameServer::GetGameInstanceContainer() -> GameInstanceContainer&
    {
        return *_gameInstanceContainer;
    }

    auto GameServer::GetCommandHandlerFactory() -> ICommandHandlerFactory&
    {
        return *_commandHandlerFactory;
    }

    auto GameServer::GetGameEntitySerializer() const -> IGameEntitySerializer&
    {
        return *_gameEntitySerializer;
    }

    void GameServer::SetPublicIP(std::string ip)
    {
        _ip = std::move(ip);
    }

    auto GameServer::SharedFromThis() -> SharedPtrNotNull<GameServer>
    {
        std::shared_ptr<Server> shared = shared_from_this();
        auto result = std::static_pointer_cast<GameServer>(shared);

        assert(result == std::dynamic_pointer_cast<GameServer>(shared));

        return result;
    }

    auto GameServer::SharedFromThis() const -> SharedPtrNotNull<const GameServer>
    {
        std::shared_ptr<const Server> shared = shared_from_this();
        auto result = std::static_pointer_cast<const GameServer>(shared);

        assert(result == std::dynamic_pointer_cast<const GameServer>(shared));

        return result;
    }

    void GameServer::OnAccept(Session& session)
    {
        ZEROSUGAR_LOG_DEBUG(_serviceLocator,
            std::format("[{}] accept session. session: {}", GetName(), session));

        {
            decltype(_sessionReceiveBuffers)::accessor accessor;

            if (_sessionReceiveBuffers.insert(accessor, session.GetId()))
            {
                accessor->second = Buffer{};
            }
            else
            {
                assert(false);

                session.Close();

                return;
            }
        }

        Delay(std::chrono::seconds(5))
            .Then(GetExecutor(), [self = SharedFromThis(), id = session.GetId(), weak = session.weak_from_this()]()
                {
                    if (self->HasClient(id))
                    {
                        return;
                    }

                    if (const auto session = weak.lock(); session)
                    {
                        session->Close();
                    }
                });
    }

    void GameServer::OnReceive(Session& session, Buffer buffer)
    {
        assert(ExecutionContext::IsEqualTo(session.GetStrand()));

        Buffer* receiveBuffer = nullptr;
        {
            decltype(_sessionReceiveBuffers)::accessor accessor;

            if (_sessionReceiveBuffers.find(accessor, session.GetId()))
            {
                receiveBuffer = &accessor->second;
            }
            else
            {
                assert(false);

                session.Close();

                return;
            }
        }

        try
        {
            receiveBuffer->MergeBack(std::move(buffer));

            while (true)
             {
                if (receiveBuffer->GetSize() < 2)
                {
                    break;
                }

                PacketReader reader(receiveBuffer->cbegin(), receiveBuffer->cend());

                const int64_t packetSize = reader.Read<int16_t>();
                if (receiveBuffer->GetSize() < packetSize)
                {
                    break;
                }

                std::unique_ptr<IPacket> packet = network::game::cs::CreateFrom(reader);

                Buffer temp;
                [[maybe_unused]] bool sliced = receiveBuffer->SliceFront(temp, packetSize);
                assert(sliced);

                if (!packet)
                {
                    ZEROSUGAR_LOG_WARN(_serviceLocator,
                        std::format("[{}] unnkown packet. session: {}", GetName(), session));

                    session.Close();

                    return;
                }

                if (const auto& handler = _gamePacketHandlerFactory->CreateHandler(packet->GetOpcode());
                    handler)
                {
                    handler->Handle(*this, session.shared_from_this(), std::move(packet));
                }
                else
                {
                    ZEROSUGAR_LOG_WARN(_serviceLocator,
                        std::format("[{}] unnkown packet. session: {}, opcode: {}", GetName(), session, packet->GetOpcode()));

                    session.Close();
                }
            }
        }
        catch (const std::exception& e)
        {
            ZEROSUGAR_LOG_WARN(_serviceLocator, std::format("[{}] throws. session: {}, exsception: {}",
                GetName(), session, e.what()));

            session.Close();
        }
    }

    void GameServer::OnError(Session& session, const boost::system::error_code& error)
    {
        ZEROSUGAR_LOG_DEBUG(_serviceLocator,
            std::format("[{}] session io error. session: {}, error: {}", GetName(), session, error.message()));

        (void)session;
        (void)error;
    }

    void GameServer::RegisterToCoordinationService()
    {
        auto& coordinationService = _serviceLocator.Get<service::ICoordinationService>();

        service::RegisterServerParam registerServerParam;
        registerServerParam.name = std::format("{}.{}:{}", GetName(), _ip, _port);
        registerServerParam.ip = _ip;
        registerServerParam.port = _port;

        service::RegisterServerResult result = coordinationService.RegisterServerAsync(std::move(registerServerParam)).Get();
        if (result.errorCode != service::CoordinationServiceErrorCode::CoordinationErrorNone)
        {
            throw std::runtime_error(std::format("fail to start game server. error: {}", GetEnumName(result.errorCode)));
        }

        _serverId = result.serverId;
    }

    void GameServer::OpenCoordinationCommandChannel()
    {
        auto& coordinationService = _serviceLocator.Get<service::ICoordinationService>();

        _responseChannel = std::make_shared<Channel<service::CoordinationCommandResponse>>();

        AsyncEnumerable<service::CoordinationCommand> commandEnumerable =
            coordinationService.OpenChannelAsync(AsyncEnumerable<service::CoordinationCommandResponse>(_responseChannel));

        _channelRunner = std::make_shared<CommandChannelRunner>(*this, GetExecutor().shared_from_this(), std::move(commandEnumerable));
        _channelRunner->Start();
    }

    void GameServer::ScheduleServerStatusReport()
    {
        _serverStatusReportStopSource = std::stop_source();

        Post(GetExecutor(), [](SharedPtrNotNull<GameServer> self, std::stop_token token) -> Future<void>
            {
                try
                {
                    while (true)
                    {
                        constexpr int32_t samplingCount = 5;
                        int32_t failToGetLoadCPUPercentageCount = 0;
                        int32_t failToGetFreePhysicalMemoryGBCount = 0;

                        int32_t loadPercentage = 0;
                        double freeMemory = 0.f;

                        for (int32_t i = 0; i < samplingCount; ++i)
                        {
                            co_await Delay(std::chrono::milliseconds(500));

                            if (token.stop_requested())
                            {
                                co_return;
                            }

                            if (const auto value = QueryCPULoadPercentage(); value.has_value())
                            {
                                loadPercentage += *value;
                            }
                            else
                            {
                                ++failToGetLoadCPUPercentageCount;
                            }

                            if (const auto value = QueryFreePhysicalMemoryGB(); value.has_value())
                            {
                                freeMemory += *value;
                            }
                            else
                            {
                                ++failToGetFreePhysicalMemoryGBCount;
                            }
                        }

                        loadPercentage = (samplingCount == failToGetLoadCPUPercentageCount) ?
                            -1 : loadPercentage / (samplingCount - failToGetLoadCPUPercentageCount);
                        freeMemory = (samplingCount == failToGetLoadCPUPercentageCount) ?
                            -1.f : freeMemory / (samplingCount - failToGetFreePhysicalMemoryGBCount);

                        service::UpdateServerStatusParam param;
                        param.serverId = self->GetServerId();
                        param.loadCPUPercentage = loadPercentage;
                        param.freePhysicalMemoryGB = freeMemory;

                        service::UpdateServerStatusResult result =
                            co_await self->_serviceLocator.Get<service::ICoordinationService>().UpdateServerStatusAsync(std::move(param));

                        if (result.errorCode != service::CoordinationServiceErrorCode::CoordinationErrorNone)
                        {
                            ZEROSUGAR_LOG_ERROR(self->_serviceLocator,
                                std::format("[{}] report server status to coordination service error. error: {}",
                                    self->GetName(), GetEnumName(result.errorCode)));

                            co_return;
                        }
                    }
                }
                catch (const std::exception& e)
                {
                    ZEROSUGAR_LOG_CRITICAL(self->_serviceLocator,
                        std::format("[{}] report server status exit by exception. exception: {}",
                            self->GetName(), e.what()));
                }

            }, SharedFromThis(), _serverStatusReportStopSource.get_token());
    }
}
