#include "all_in_one_app.h"

#include "zerosugar/shared/database/connection/connection_pool.h"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger_builder.h"
#include "zerosugar/xr/application/all_in_one_app/all_in_one_app_config.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"
#include "zerosugar/xr/network/rpc/rpc_server.h"
#include "zerosugar/xr/network/server/login_server.h"
#include "zerosugar/xr/service/database/database_service.h"
#include "zerosugar/xr/service/orchestrator/orchestrator_service.h"
#include "zerosugar/xr/service/login/login_service.h"

namespace zerosugar::xr
{
    AllInOneApp::AllInOneApp(const AllInOneAppConfig& config)
        : _config(std::make_unique<AllInOneAppConfig>(config))
        , _executor(std::make_shared<execution::AsioExecutor>(_config->workerCount))
        , _logService(std::make_shared<LogService>())
        , _connectionPool(std::make_shared<db::ConnectionPool>(_executor))
        , _rpcServer(std::make_shared<RPCServer>(_executor))
        , _rpcClient(std::make_shared<RPCClient>(_executor))
        , _loginServer(std::make_shared<LoginServer>(*_executor))
        , _orchestratorService(std::make_shared<OrchestratorService>(_executor))
        , _loginService(std::make_shared<LoginService>(_executor))
        , _loginServiceProxy(std::make_shared<service::LoginServiceProxy>(_rpcClient))
        , _databaseService(std::make_shared<DatabaseService>(_executor, _connectionPool))
        , _databaseServiceProxy(std::make_shared<service::DatabaseServiceProxy>(_rpcClient))
    {
        ServiceLocator& serviceLocator = GetServiceLocator();
        
        serviceLocator.Add<ILogService>(_logService);

        serviceLocator.Add<RPCServer>(_rpcServer);
        serviceLocator.Add<RPCClient>(_rpcClient);

        serviceLocator.Add<service::IOrchestratorService>(_orchestratorService);
        serviceLocator.Add<service::ILoginService>(_loginServiceProxy);
        serviceLocator.Add<service::IDatabaseService>(_databaseServiceProxy);
    }

    AllInOneApp::~AllInOneApp()
    {
    }

    void AllInOneApp::OnStartUp(std::span<char*> args)
    {
        (void)args;

        InitializeConfig();
        InitializeLogger();
        InitializeExecutor();
        InitializeDatabaseConnection();

        ServiceLocator& serviceLocator = GetServiceLocator();

        InitializeService(serviceLocator);
        InitializeNetwork(serviceLocator);
    }

    void AllInOneApp::OnShutdown()
    {
    }

    void AllInOneApp::InitializeConfig()
    {
        auto path = std::filesystem::current_path();
        for (; exists(path) && path != path.parent_path(); path = path.parent_path())
        {
            std::filesystem::path configPath = path / "zerosugar/xr/application/all_in_one_app/all_in_one_app.config";
            if (!exists(configPath))
            {
                continue;
            }

            std::ifstream ifs(configPath);
            if (!ifs.is_open())
            {
                throw std::runtime_error("fail to open config file");
            }

            nlohmann::json json = nlohmann::json::parse(ifs);
            json.get_to<AllInOneAppConfig>(*_config);

            return;
        }

        throw std::runtime_error("fail to initialize config");
    }

    auto AllInOneApp::GetName() const -> std::string_view
    {
        return "all_in_one_application";
    }

    void AllInOneApp::InitializeLogger()
    {
        const std::filesystem::path& logFilePath = std::filesystem::current_path() / _config->logFilePath;

        SpdLogLoggerBuilder builder;
        builder.ConfigureConsole().SetLogLevel(LogLevel::Info).SetAsync(false);
        builder.ConfigureDailyFile().SetLogLevel(LogLevel::Debug).SetPath(logFilePath);

        _logService->Add(-1, builder.CreateLogger());

        GetServiceLocator().Add<ILogService>(_logService);

        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize logger --> Done. log_file_path: {}", GetName(), logFilePath.string()));
    }

    void AllInOneApp::InitializeExecutor()
    {
        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize executor", GetName()));

        ExecutionContext::PushExecutor(_executor.get());
        _executor->Run();

        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize executor --> Done", GetName()));
    }

    void AllInOneApp::InitializeDatabaseConnection()
    {
        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize database connection", GetName()));

        const auto endPoint = boost::asio::ip::tcp::endpoint(
            boost::asio::ip::make_address(_config->databaseIP), _config->databasePort);
        const boost::mysql::handshake_params params(_config->databaseUser, _config->databasePassword, _config->databaseSchema);

        db::ConnectionPoolOption option(endPoint, params, _config->databaseConnectionCount);

        _connectionPool->Initialize(GetServiceLocator());
        _connectionPool->Start(option);

        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize database connection --> Done", GetName()));
    }

    void AllInOneApp::InitializeService(ServiceLocator& serviceLocator)
    {
        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize service", GetName()));

        _orchestratorService->Initialize(serviceLocator);
        _loginService->Initialize(serviceLocator);
        _databaseService->Initialize(serviceLocator);

        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize service --> Done", GetName()));
    }

    void AllInOneApp::InitializeNetwork(ServiceLocator& serviceLocator)
    {
        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize network", GetName()));

        _rpcServer->Initialize(serviceLocator);
        _rpcClient->Initialize(serviceLocator);
        _loginServer->Initialize(serviceLocator);

        _rpcServer->StartUp(_config->rpcServerPort);
        assert(_rpcServer->IsOpen());
        {
            Future<void> future = _rpcClient->ConnectAsync(_config->rpcServerIP, _config->rpcServerPort);

            while (future.IsPending())
            {
                std::this_thread::sleep_for(std::chrono::seconds(3));

                ZEROSUGAR_LOG_INFO(serviceLocator, std::format("[rpc_client] pending connection to rpc server..."));
            }

            future.Get();

            ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[rpc_client] connecting to rpc server --> Done"));


            const char* loginService = decltype(_loginService)::element_type::name;
            _rpcClient->RegisterToServer(loginService, _config->rpcServerIP, _config->rpcServerPort).Get();
            ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[rpc_client] register {} to rpc server --> Done", loginService));

            const char* databaseService = decltype(_databaseService)::element_type::name;
            _rpcClient->RegisterToServer(databaseService, _config->rpcServerIP, _config->rpcServerPort).Get();
            ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[rpc_client] register {} to rpc server --> Done", databaseService));
        }

        // TODO: remove hardcoding
        _loginServer->StartUp(8181);

        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize network --> Done", GetName()));
        
    }
}
