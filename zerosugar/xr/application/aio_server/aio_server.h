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
    struct AIOServerConfig;

    class OrchestratorService;
    class LoginService;
    class LoginServiceProxy;

    class AIOServer final : public AppInstance
    {
    public:
        AIOServer() = delete;
        explicit AIOServer(const AIOServerConfig& config);

    private:
        void OnStartUp(std::span<char*> args) override;
        void OnShutdown() override;

    public:
        auto GetName() const -> std::string_view override;

    private:
        std::unique_ptr<AIOServerConfig> _config;

        SharedPtrNotNull<execution::AsioExecutor> _executor;

        // network
        SharedPtrNotNull<RPCServer> _rpcServer;
        SharedPtrNotNull<RPCClient> _rpcClient;

        // service
        SharedPtrNotNull<OrchestratorService> _orchestratorService;

        SharedPtrNotNull<LoginService> _loginService;
        SharedPtrNotNull<LoginServiceProxy> _loginServiceProxy;
    };
}
