#include <boost/lexical_cast.hpp>

#include "zerosugar/shared/app/app_intance.h"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger_builder.h"
#include "zerosugar/xr/server/lobby/lobby_server.h"
#include "zerosugar/xr/service/model/generated/login_service.h"
#include "zerosugar/xr/service/model/generated/gateway_service.h"
#include "zerosugar/xr/service/model/generated/database_service.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"

namespace zerosugar::xr
{
    class LobbyServerApplication final : public AppInstance
    {
    public:
        LobbyServerApplication();

        auto GetName() const->std::string_view override;

    private:
        void OnStartUp(std::span<char*> args) override;
        void OnShutdown() override;

    private:
        SharedPtrNotNull<execution::AsioExecutor> _executor;
        SharedPtrNotNull<RPCClient> _rpcClient;
        SharedPtrNotNull<LobbyServer> _lobbyServer;
        SharedPtrNotNull<service::LoginServiceProxy> _loginServiceProxy;
        SharedPtrNotNull<service::GatewayServiceProxy> _gatewayServiceProxy;
        SharedPtrNotNull<service::DatabaseServiceProxy> _databaseServiceProxy;
        SharedPtrNotNull<service::CoordinationServiceProxy> _coordinationServiceProxy;
    };

    LobbyServerApplication::LobbyServerApplication()
        : _executor(std::make_shared<execution::AsioExecutor>(4))
        , _rpcClient(std::make_shared<RPCClient>(_executor))
        , _lobbyServer(std::make_shared<LobbyServer>(*_executor))
        , _loginServiceProxy(std::make_shared<service::LoginServiceProxy>(_rpcClient))
        , _gatewayServiceProxy(std::make_shared<service::GatewayServiceProxy>(_rpcClient))
        , _databaseServiceProxy(std::make_shared<service::DatabaseServiceProxy>(_rpcClient))
        , _coordinationServiceProxy(std::make_shared<service::CoordinationServiceProxy>(_rpcClient))
    {
        ExecutionContext::PushExecutor(_executor.get());
    }

    auto LobbyServerApplication::GetName() const -> std::string_view
    {
        return "lobby_server_application";
    }

    void LobbyServerApplication::OnStartUp(std::span<char*> args)
    {
        ServiceLocator& serviceLocator = GetServiceLocator();

        SpdLogLoggerBuilder builder;
        builder.ConfigureConsole().SetLogLevel(LogLevel::Info).SetAsync(false);

        auto temp = std::make_shared<LogService>();
        temp->Add(-1, builder.CreateLogger());

        serviceLocator.Add<ILogService>(std::move(temp));
        serviceLocator.Add<service::ILoginService>(_loginServiceProxy);
        serviceLocator.Add<service::IGatewayService>(_gatewayServiceProxy);
        serviceLocator.Add<service::IDatabaseService>(_databaseServiceProxy);
        serviceLocator.Add<service::ICoordinationService>(_coordinationServiceProxy);

        ZEROSUGAR_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] command lines args: {}",
                GetName(), [&]() -> std::string
                {
                    std::ostringstream oss;

                    for (const char* arg : args)
                    {
                        oss << arg << ' ';
                    }

                    std::string result = oss.str();
                    if (!result.empty())
                    {
                        result.pop_back();
                    }

                    return result;
                }()));

        if (args.size() < 3)
        {
            throw std::runtime_error("command line { args[1], args[2] } -> rpc server endpoint is null");
        }

        const std::string address(args[1]);
        const uint16_t port = boost::lexical_cast<uint16_t>(args[2]);

        _executor->Run();

        Future<void> future = _rpcClient->ConnectAsync(address, port);

        auto lastTimePoint = std::chrono::system_clock::now();

        while (future.IsPending())
        {
            const auto now = std::chrono::system_clock::now();

            if (now - lastTimePoint >= std::chrono::seconds(3))
            {
                ZEROSUGAR_LOG_INFO(GetServiceLocator(),
                    fmt::format("[{}] try to connect rpc server...",
                        GetName()));

                lastTimePoint = now;
            }
        }

        _lobbyServer->Initialize(GetServiceLocator());
        _lobbyServer->SetPublicIP("127.0.0.1");
        _lobbyServer->StartUp(8182);
    }

    void LobbyServerApplication::OnShutdown()
    {

    }
}

int main(int argc, char* argv[])
{
    using namespace zerosugar::xr;

    LobbyServerApplication application;

    return application.Run(std::span(argv, argc));
}
