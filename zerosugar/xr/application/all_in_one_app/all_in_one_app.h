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
    class LobbyServer;
}

namespace zerosugar::xr
{
    class LoginService;
    class GatewayService;
    class CoordinationService;
    class DatabaseService;

    namespace service
    {
        class LoginServiceProxy;
        class GatewayServiceProxy;
        class CoordinationServiceProxy;
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
        void InitializeServer(ServiceLocator& serviceLocator);

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
        SharedPtrNotNull<LobbyServer> _lobbyServer;

        // service
        std::vector<SharedPtrNotNull<IService>> _services;

        SharedPtrNotNull<LoginService> _loginService;
        SharedPtrNotNull<service::LoginServiceProxy> _loginServiceProxy;

        SharedPtrNotNull<GatewayService> _gatewayService;
        SharedPtrNotNull<service::GatewayServiceProxy> _gatewayServiceProxy;

        SharedPtrNotNull<CoordinationService> _coordinationService;
        SharedPtrNotNull<service::CoordinationServiceProxy> _coordinationServiceProxy;

        SharedPtrNotNull<DatabaseService> _databaseService;
        SharedPtrNotNull<service::DatabaseServiceProxy> _databaseServiceProxy;
    };
}
