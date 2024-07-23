#include "game_server.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/shared/process/query_sys_info.h"
#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/client/game_client.h"
#include "zerosugar/xr/server/game/command/command_channel_runner.h"
#include "zerosugar/xr/server/game/command/command_handler_factory.h"
#include "zerosugar/xr/server/game/client/game_client_container.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/game_instance_container.h"
#include "zerosugar/xr/server/game/packet/packet_handler_factory.h"
#include "zerosugar/xr/server/game/packet/packet_handler_interface.h"
#include "zerosugar/xr/server/game/repository/game_repository.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_despawn.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"

namespace zerosugar::xr
{
    GameServer::GameServer(execution::AsioExecutor& ioExecutor, execution::IExecutor& gameExecutor)
        : Server("game_server", ioExecutor)
        , _gameExecutor(gameExecutor)
        , _clientContainer(std::make_unique<GameClientContainer>())
        , _gameInstanceContainer(std::make_unique<GameInstanceContainer>())
        , _gamePacketHandlerFactory(std::make_unique<GamePacketHandlerFactory>())
        , _commandHandlerFactory(std::make_unique<CommandHandlerFactory>())
    {
    }

    GameServer::~GameServer()
    {
    }

    void GameServer::Initialize(ServiceLocator& serviceLocator)
    {
        Server::Initialize(serviceLocator);

        _serviceLocator = serviceLocator;
        _gameRepository = std::make_shared<GameRepository>(GetExecutor(), _serviceLocator,
            [self = SharedFromThis()](int64_t characterId)
            {
                self->OnCharacterSaveError(characterId);
            });

        _serviceLocator.Add<IGameRepository>(_gameRepository);
    }

    void GameServer::StartUp(uint16_t listenPort)
    {
        Server::StartUp(listenPort);

        _port = listenPort;

        ConfigureSnowflake();
        RegisterToCoordinationService();
        OpenCoordinationCommandChannel();
        //ScheduleServerStatusReport();
        ScheduleServerStatusReport();
    }

    void GameServer::Shutdown()
    {
        Server::Shutdown();

        if (_deviceStatusReportStopSource.stop_possible())
        {
            _deviceStatusReportStopSource.request_stop();
        }

        if (_serverStatusReportStopSource.stop_possible())
        {
            _serverStatusReportStopSource.request_stop();
        }
    }

    bool GameServer::HasClient(session::id_type id) const
    {
        return _clientContainer->Contains(id);
    }

    bool GameServer::AddClient(session::id_type id, SharedPtrNotNull<GameClient> client)
    {
        return _clientContainer->Add(id, std::move(client));
    }

    auto GameServer::FindClient(session::id_type id) const -> SharedPtrNotNull<GameClient>
    {
        return _clientContainer->Find(id);
    }

    auto GameServer::ReleaseClient(int64_t userId) -> Future<void>
    {
        std::shared_ptr<GameClient> client = _clientContainer->FindByUserId(userId);
        if (!client)
        {
            co_return;
        }

        std::shared_ptr<GameInstance> instance = client->GetGameInstance();
        if (!instance)
        {
            co_return;
        }

        Promise<void> promise;
        Future<void> future = promise.GetFuture();

        auto task = std::make_unique<game_task::PlayerDepsawn>(std::move(promise), client->GetControllerId(), client->GetGameEntityId());
        instance->Summit(std::move(task), std::nullopt);

        co_await future;
        co_await _gameRepository->FinalizeSaves(client->GetCharacterId());

        _clientContainer->Remove(userId);

        co_return;
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

    auto GameServer::GetGameExecutor() -> execution::IExecutor&
    {
        return _gameExecutor;
    }

    auto GameServer::GetClientContainer() -> GameClientContainer&
    {
        return *_clientContainer;
    }

    auto GameServer::GetGameInstanceContainer() -> GameInstanceContainer&
    {
        return *_gameInstanceContainer;
    }

    auto GameServer::GetCommandHandlerFactory() -> ICommandHandlerFactory&
    {
        return *_commandHandlerFactory;
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
            fmt::format("[{}] accept session. session: {}", GetName(), session));

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

            while (receiveBuffer->GetSize() >= 4)
            {
                PacketReader reader(receiveBuffer->cbegin(), receiveBuffer->cend());

                const int64_t packetSize = reader.Read<int32_t>();
                if (receiveBuffer->GetSize() < packetSize)
                {
                    break;
                }

                std::unique_ptr<IPacket> packet = network::game::cs::CreateFrom(reader);
                _receivePacketCount.fetch_add(1);

                Buffer temp;
                [[maybe_unused]] bool sliced = receiveBuffer->SliceFront(temp, packetSize);
                assert(sliced);

                if (!packet)
                {
                    ZEROSUGAR_LOG_WARN(_serviceLocator,
                        fmt::format("[{}] unnkown packet. session: {}", GetName(), session));

                    //session.Close();

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
                        fmt::format("[{}] unnkown packet. session: {}, opcode: {}", GetName(), session, packet->GetOpcode()));

                    //session.Close();
                }
            }
        }
        catch (const std::exception& e)
        {
            ZEROSUGAR_LOG_WARN(_serviceLocator, fmt::format("[{}] throws. session: {}, exsception: {}",
                GetName(), session, e.what()));

            session.Close();
        }
    }

    void GameServer::OnError(Session& session, const boost::system::error_code& error)
    {
        ZEROSUGAR_LOG_DEBUG(_serviceLocator,
            fmt::format("[{}] session io error. session: {}, error: {}",
                GetName(), session, error.message()));

        _sessionReceiveBuffers.erase(session.GetId());

        std::shared_ptr<GameClient> client = _clientContainer->Find(session.GetId());
        if (!client)
        {
            return;
        }

        ProcessClientRemove(std::move(client));
    }

    void GameServer::OnCharacterSaveError(int64_t characterId)
    {
        ZEROSUGAR_LOG_ERROR(GetServiceLocator(),
            fmt::format("[{}] character save error occur. character_id: {}",
                GetName(), characterId));

        std::shared_ptr<GameClient> client = _clientContainer->FindByCharacterId(characterId);
        if (!client)
        {
            ZEROSUGAR_LOG_CRITICAL(GetServiceLocator(),
                fmt::format("[{}] fail to find client. character_id: {}",
                    GetName(), characterId));

            return;
        }

        ProcessClientRemove(std::move(client));
    }

    auto GameServer::ProcessClientRemove(SharedPtrNotNull<GameClient> client) -> Future<void>
    {
        const session::id_type sessionId = client->GetSessionId();

        [[maybe_unused]]
        const bool removed = _clientContainer->Remove(sessionId);
        assert(removed);

        std::shared_ptr<GameInstance> gameInstance = client->GetGameInstance();
        assert(gameInstance);

        service::RemovePlayerParam param;
        param.serverId = GetServerId();
        param.authenticationToken = client->GetAuthenticationToken();

        auto removeFuture = _serviceLocator.Get<service::ICoordinationService>().RemovePlayerAsync(std::move(param));
        auto finalizeSaveFuture = _gameRepository->FinalizeSaves(client->GetCharacterId());

        Promise<void> promise;
        Future<void> despawnFuture = promise.GetFuture();

        auto task = std::make_unique<game_task::PlayerDepsawn>(std::move(promise), client->GetControllerId(), client->GetGameEntityId());
        gameInstance->Summit(std::move(task), std::nullopt);

        co_await WaitAll(GetGameExecutor(), removeFuture, finalizeSaveFuture, despawnFuture);

        client->Shutdown();

        service::RemovePlayerResult removeResult = removeFuture.Get();
        if (removeResult.errorCode != service::CoordinationServiceErrorCode::CoordinationErrorNone)
        {
            ZEROSUGAR_LOG_ERROR(GetServiceLocator(),
                fmt::format("[{}] fail to remove client auth. chracter_id: {}, auth: {}, error: {}",
                    GetName(), client->GetCharacterId(), client->GetAuthenticationToken(), GetEnumName(removeResult.errorCode)));
        }

        ZEROSUGAR_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] client is removed from game. character_id: {}, auth: {}",
                GetName(), client->GetCharacterId(), client->GetAuthenticationToken()));

        co_return;
    }

    void GameServer::ConfigureSnowflake()
    {
        auto& coordinationService = _serviceLocator.Get<service::ICoordinationService>();

        service::RequestSnowflakeKeyParam requestSnowflakeKeyParam;
        requestSnowflakeKeyParam.requester = GetName();

        const service::RequestSnowflakeKeyResult res = coordinationService.RequestSnowflakeKeyAsync(std::move(requestSnowflakeKeyParam)).Get();
        if (res.errorCode != service::CoordinationServiceErrorCode::CoordinationErrorNone)
        {
            ZEROSUGAR_LOG_CRITICAL(_serviceLocator,
                fmt::format("[{}] fail to get snowflake key: {}", GetName(), GetEnumName(res.errorCode)));

            return;
        }

        assert(_gameRepository);

        _gameRepository->SetSnowFlake(std::make_unique<SharedSnowflake<>>(res.snowflakeKey));
    }

    void GameServer::RegisterToCoordinationService()
    {
        auto& coordinationService = _serviceLocator.Get<service::ICoordinationService>();

        service::RegisterServerParam registerServerParam;
        registerServerParam.name = fmt::format("{}.{}:{}", GetName(), _ip, _port);
        registerServerParam.ip = _ip;
        registerServerParam.port = _port;

        service::RegisterServerResult result = coordinationService.RegisterServerAsync(std::move(registerServerParam)).Get();
        if (result.errorCode != service::CoordinationServiceErrorCode::CoordinationErrorNone)
        {
            throw std::runtime_error(fmt::format("fail to start game server. error: {}", GetEnumName(result.errorCode)));
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

    void GameServer::ScheduleDeviceStatusReport()
    {
        _deviceStatusReportStopSource = std::stop_source();

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
                                fmt::format("[{}] report server status to coordination service error. error: {}",
                                    self->GetName(), GetEnumName(result.errorCode)));

                            co_return;
                        }
                    }
                }
                catch (const std::exception& e)
                {
                    ZEROSUGAR_LOG_CRITICAL(self->_serviceLocator,
                        fmt::format("[{}] report server status exit by exception. exception: {}",
                            self->GetName(), e.what()));
                }

            }, SharedFromThis(), _serverStatusReportStopSource.get_token());
    }

    void GameServer::ScheduleServerStatusReport()
    {
        _serverStatusReportStopSource = std::stop_source();

        Post(GetExecutor(), [](SharedPtrNotNull<GameServer> self, std::stop_token token) -> Future<void>
            {
                while (true)
                {
                    for (int64_t i = 0; i < 5; ++i)
                    {
                        co_await Delay(std::chrono::seconds(1));

                        if (token.stop_requested())
                        {
                            break;
                        }
                    }

                    int64_t value = 0;
                    const int64_t packetCount = self->_receivePacketCount.exchange(value);

                    ZEROSUGAR_LOG_INFO(self->GetServiceLocator(),
                        fmt::format("[{}] session: {}, receive packet: {}, game_instance: {}",
                            self->GetName(), self->GetClientContainer().GetCount(), packetCount, self->_gameInstanceContainer->GetCount()));
                }

            }, SharedFromThis(), _serverStatusReportStopSource.get_token());;
    }
}
