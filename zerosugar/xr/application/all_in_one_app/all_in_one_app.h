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

    public:
        auto GetName() const -> std::string_view override;

    private:
        std::unique_ptr<AllInOneAppConfig> _config;

        SharedPtrNotNull<execution::AsioExecutor> _executor;

        // network
        SharedPtrNotNull<RPCServer> _rpcServer;
        SharedPtrNotNull<RPCClient> _rpcClient;
        // SharedPtrNotNull<GameServer> _gameServer;

        // service
        SharedPtrNotNull<OrchestratorService> _orchestratorService;
        SharedPtrNotNull<OrchestratorServiceProxy> _orchestratorServiceProxy;

        SharedPtrNotNull<LoginService> _loginService;
        SharedPtrNotNull<LoginServiceProxy> _loginServiceProxy;
    };
}
