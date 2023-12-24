#pragma once
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/sl/executable/monolithic_server/module/module_interface.h"

namespace zerosugar
{
    class Server;
}

namespace zerosugar::sl
{
    class ServerModule : public IModule
    {
    public:
        void Initialize(AppInstance& app, AppConfig& config) override;
        void Finalize() noexcept override;
        void GetFinalizeError(std::vector<boost::system::error_code>& errors) override;

    private:
        void InitializeServerExecutor();
        void InitializeGatewayServers(AppInstance& app, AppConfig& config);
        void InitializeZoneServers(AppInstance& app, AppConfig& config);
        void InitializeLoginServers(AppInstance& app, AppConfig& config);

    private:
        SharedPtrNotNull<execution::AsioExecutor> _executor;
        std::map<int8_t, std::vector<SharedPtrNotNull<Server>>> _worldZoneServers;
        std::map<int8_t, SharedPtrNotNull<Server>> _gatewayServers;
        SharedPtrNotNull<Server> _loginServer;
    };
}
