#include "all_in_one_app.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger_builder.h"
#include "zerosugar/xr/application/all_in_one_app/all_in_one_app_config.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"
#include "zerosugar/xr/network/rpc/rpc_server.h"
#include "zerosugar/xr/service/orchestrator/orchestrator_service.h"
#include "zerosugar/xr/service/login/login_service.h"
#include "zerosugar/xr/service/login/login_service_proxy.h"

namespace zerosugar::xr
{
    AllInOneApp::AllInOneApp(const AllInOneAppConfig& config)
        : _config(std::make_unique<AllInOneAppConfig>(config))
        , _executor(std::make_shared<execution::AsioExecutor>(_config->workerCount))
        , _logService(std::make_shared<LogService>())
        , _rpcServer(std::make_shared<RPCServer>(_executor))
        , _rpcClient(std::make_shared<RPCClient>(_executor))
        , _orchestratorService(std::make_shared<OrchestratorService>(_executor))
        , _loginService(std::make_shared<LoginService>(_executor))
        , _loginServiceProxy(std::make_shared<LoginServiceProxy>(_rpcClient))
    {
    }

    AllInOneApp::~AllInOneApp()
    {
    }

    void AllInOneApp::OnStartUp(std::span<char*> args)
    {
        (void)args;

        InitializeExecutor();
        InitializeLogger();
        InitializeServiceLocator();

        ServiceLocator& serviceLocator = GetServiceLocator();

        InitializeService(serviceLocator);
        InitializeNetwork(serviceLocator);
    }

    void AllInOneApp::OnShutdown()
    {
    }

    auto AllInOneApp::GetName() const -> std::string_view
    {
        return "all_in_one_application";
    }

    void AllInOneApp::InitializeExecutor()
    {
        ExecutionContext::PushExecutor(_executor.get());
        _executor->Run();
    }

    void AllInOneApp::InitializeLogger()
    {
        // TODO: remove hardcoding
        SpdLogLoggerBuilder builder;
        builder.ConfigureConsole().SetLogLevel(LogLevel::Info).SetAsync(false);
        builder.ConfigureDailyFile().SetLogLevel(LogLevel::Debug).SetPath(std::filesystem::current_path() / "logs/log.txt");

        _logService->Add(-1, builder.CreateLogger());
    }

    void AllInOneApp::InitializeServiceLocator()
    {
        ServiceLocator& serviceLocator = GetServiceLocator();

        serviceLocator.Add<ILogService>(_logService);

        serviceLocator.Add<RPCServer>(_rpcServer);
        serviceLocator.Add<RPCClient>(_rpcClient);

        serviceLocator.Add<service::IOrchestratorService>(_orchestratorService);
        serviceLocator.Add<service::ILoginService>(_loginServiceProxy);
    }

    void AllInOneApp::InitializeService(ServiceLocator& serviceLocator)
    {
        _orchestratorService->Initialize(serviceLocator);
        _loginService->Initialize(serviceLocator);
    }

    void AllInOneApp::InitializeNetwork(ServiceLocator& serviceLocator)
    {
        _rpcServer->Initialize(serviceLocator);
        _rpcClient->Initialize(serviceLocator);

        _rpcServer->StartUp(_config->rpcServerPort);
        assert(_rpcServer->IsOpen());

        Future<void> future = _rpcClient->ConnectAsync(_config->rpcServerIP, _config->rpcServerPort);

        while (future.IsPending())
        {
            std::this_thread::sleep_for(std::chrono::seconds(3));

            ZEROSUGAR_LOG_INFO(serviceLocator, std::format("[rpc_client] pending connection to rpc server..."));
        }

        future.Get();
    }
}
