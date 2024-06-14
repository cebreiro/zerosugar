#pragma once
#include "zerosugar/xr/service/model/generated/database_service.h"

namespace zerosugar::db
{
    class ConnectionPool;
}

namespace zerosugar::xr
{
    class DatabaseService final
        : public service::IDatabaseService
        , public std::enable_shared_from_this<DatabaseService>
    {
    public:
        DatabaseService(SharedPtrNotNull<execution::IExecutor> executor, SharedPtrNotNull<db::ConnectionPool> connectionPool);

        void Initialize(ServiceLocator& serviceLocator) override;
        void Shutdown() override;
        void Join(std::vector<boost::system::error_code>& errors) override;

        auto AddAccountAsync(service::AddAccountParam param) -> Future<service::AddAccountResult> override;
        auto GetAccountAsync(service::GetAccountParam param) -> Future<service::GetAccountResult> override;

    private:
        void ConfigureRemoteProcedureClient(RPCClient& rpcClient);

    private:
        SharedPtrNotNull<execution::IExecutor> _executor;
        SharedPtrNotNull<db::ConnectionPool> _connectionPool;

        ServiceLocatorT<ILogService> _serviceLocator;
    };
}
