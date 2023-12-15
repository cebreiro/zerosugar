#pragma once
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/sl/executable/monolithic_server/module/module_interface.h"

namespace zerosugar::sl::db
{
    class ConnectionPool;
}

namespace zerosugar::sl::service
{
    class ILoginService;
    class IRepositoryService;
}

namespace zerosugar::sl
{
    class ServiceModule : public IModule
    {
    public:
        void Initialize(AppInstance& app, AppConfig& config) override;
        void Finalize() noexcept override;
        void GetFinalizeError(std::vector<boost::system::error_code>& errors) override;

    private:
        SharedPtrNotNull<execution::AsioExecutor> _dbExecutor;
        SharedPtrNotNull<db::ConnectionPool> _connectionPool;
        SharedPtrNotNull<service::IRepositoryService> _repositoryService;

        SharedPtrNotNull<execution::AsioExecutor> _executor;
        SharedPtrNotNull<service::ILoginService> _loginService;
    };
}
