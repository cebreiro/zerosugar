#include "game_server.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/process/query_sys_info.h"
#include "zerosugar/xr/server/game/command/command_channel_runner.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"

namespace zerosugar::xr
{
    GameServer::GameServer(execution::AsioExecutor& executor, std::string ip)
        : Server("game_server", executor)
        , _ip(std::move(ip))
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
        ScheduleServerStatusReport();
    }

    void GameServer::Shutdown()
    {
        Server::Shutdown();

        if (_serverStatusReportStopSource.stop_possible())
        {
            _serverStatusReportStopSource.request_stop();
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
        (void)session;
    }

    void GameServer::OnReceive(Session& session, Buffer buffer)
    {
        (void)session;
        (void)buffer;
    }

    void GameServer::OnError(Session& session, const boost::system::error_code& error)
    {
        (void)session;
        (void)error;
    }

    void GameServer::RegisterToCoordinationService()
    {
        auto& coordinationService = _serviceLocator.Get<service::ICoordinationService>();

        service::RegisterServerParam registerServerParam;
        registerServerParam.name = std::format("{}.{}:{}", GetName(), _ip, _port);

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
                while (true)
                {
                    constexpr int64_t samplingCount = 5;

                    int32_t loadPercentage = 0;
                    double freeMemory = 0.f;

                    for (int64_t i = 0; i < samplingCount; ++i)
                    {
                        co_await Delay(std::chrono::milliseconds(1000));

                        if (token.stop_requested())
                        {
                            co_return;
                        }

                        loadPercentage += QueryCPULoadPercentage();
                        freeMemory += QueryFreePhysicalMemoryGB();
                    }

                    loadPercentage /= samplingCount;
                    freeMemory /= samplingCount;

                    service::UpdateServerStatusParam param;
                    param.loadCPUPercentage = loadPercentage;
                    param.freePhysicalMemoryGB = freeMemory;

                    (void)self->_serviceLocator.Get<service::ICoordinationService>().UpdateServerStatusAsync(std::move(param));
                }

            }, SharedFromThis(), _serverStatusReportStopSource.get_token());
    }
}
