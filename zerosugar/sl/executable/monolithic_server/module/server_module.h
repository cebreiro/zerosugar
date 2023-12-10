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
        void Initialize(AppInstance& app, ServerConfig& config) override;
        void Finalize() noexcept override;
        void GetFinalizeError(std::vector<boost::system::error_code>& errors) override;

    private:
        SharedPtrNotNull<execution::AsioExecutor> _executor;
        SharedPtrNotNull<Server> _loginServer;
    };
}