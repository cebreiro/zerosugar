#pragma once
#include "zerosugar/shared/app/app_intance.h"

namespace zerosugar::db
{
    class ConnectionPool;
}

namespace zerosugar::execution
{
    class AsioExecutor;
}

namespace zerosugar::xr
{
    class RPCServer;
    class RPCClient;
    class LoginServer;
}

namespace zerosugar::xr
{
    class OrchestratorService;
    class LoginService;
    class DatabaseService;

    namespace service
    {
        class OrchestratorServiceProxy;
        class LoginServiceProxy;
        class DatabaseServiceProxy;
    }
}

namespace zerosugar::xr
{
    struct AllInOneAppConfig;

    class AllInOneApp final : public AppInstance
    {
    public:
        AllInOneApp() = delete;
        explicit AllInOneApp(const AllInOneAppConfig& config);
        ~AllInOneApp();

    private:
        void OnStartUp(std::span<char*> args) override;
        void OnShutdown() override;

    private:
        void InitializeConfig();
        void InitializeLogger();
        void InitializeExecutor();
        void InitializeDatabaseConnection();
        void InitializeService(ServiceLocator& serviceLocator);
        void InitializeNetwork(ServiceLocator& serviceLocator);

    public:
        auto GetName() const -> std::string_view override;

    private:
        std::unique_ptr<AllInOneAppConfig> _config;

        SharedPtrNotNull<execution::AsioExecutor> _executor;
        SharedPtrNotNull<LogService> _logService;
        SharedPtrNotNull<db::ConnectionPool> _connectionPool;

        // network
        SharedPtrNotNull<RPCServer> _rpcServer;
        SharedPtrNotNull<RPCClient> _rpcClient;
        SharedPtrNotNull<LoginServer> _loginServer;

        // service
        SharedPtrNotNull<OrchestratorService> _orchestratorService;
        SharedPtrNotNull<service::OrchestratorServiceProxy> _orchestratorServiceProxy;

        SharedPtrNotNull<LoginService> _loginService;
        SharedPtrNotNull<service::LoginServiceProxy> _loginServiceProxy;

        SharedPtrNotNull<DatabaseService> _databaseService;
        SharedPtrNotNull<service::DatabaseServiceProxy> _databaseServiceProxy;
    };
}
