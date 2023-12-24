#pragma once
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/sl/executable/monolithic_server/assembler/assembler_interface.h"

namespace zerosugar::sl::db
{
    class ConnectionPool;
}

namespace zerosugar::sl::service
{
    class ILoginService;
    class IWorldService;
    class IRepositoryService;
}

namespace zerosugar::sl
{
    class ServiceAssembler : public IAssembler
    {
    public:
        void Initialize(AppInstance& app, AppConfig& config) override;
        void Finalize() noexcept override;
        void GetFinalizeError(std::vector<boost::system::error_code>& errors) override;

    private:
        SharedPtrNotNull<execution::AsioExecutor> _dbExecutor;
        SharedPtrNotNull<db::ConnectionPool> _connectionPool;
        SharedPtrNotNull<execution::AsioExecutor> _executor;

        SharedPtrNotNull<service::IRepositoryService> _repositoryService;
        SharedPtrNotNull<service::IWorldService> _worldService;
        SharedPtrNotNull<service::ILoginService> _loginService;
    };
}
