#pragma once
#include "zerosugar/shared/app/app_intance.h"

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
    struct AllInOneAppConfig;

    class OrchestratorService;
    class OrchestratorServiceProxy;
    class LoginService;
    class LoginServiceProxy;

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
        void InitializeExecutor();
        void InitializeLogger();
        void InitializeServiceLocator();
        void InitializeService(ServiceLocator& serviceLocator);
        void InitializeNetwork(ServiceLocator& serviceLocator);

    public:
        auto GetName() const -> std::string_view override;

    private:
        std::unique_ptr<AllInOneAppConfig> _config;

        SharedPtrNotNull<execution::AsioExecutor> _executor;
        SharedPtrNotNull<LogService> _logService;

        // network
        SharedPtrNotNull<RPCServer> _rpcServer;
        SharedPtrNotNull<RPCClient> _rpcClient;
        SharedPtrNotNull<LoginServer> _loginServer;

        // service
        SharedPtrNotNull<OrchestratorService> _orchestratorService;
        SharedPtrNotNull<OrchestratorServiceProxy> _orchestratorServiceProxy;

        SharedPtrNotNull<LoginService> _loginService;
        SharedPtrNotNull<LoginServiceProxy> _loginServiceProxy;
    };
}
