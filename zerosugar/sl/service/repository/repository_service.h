#pragma once
#include "zerosugar/sl/database/model/account.h"
#include "zerosugar/sl/service/generated/repository_service_generated_interface.h"

namespace zerosugar::execution
{
    class IExecutor;
}

namespace zerosugar::sl::db
{
    class ConnectionPool;
}

namespace zerosugar::sl
{
    class RepositoryService
        : public service::IRepositoryService
        , public std::enable_shared_from_this<RepositoryService>
    {
    public:
        using locator_type = ServiceLocatorRef<ILogService>;

    public:
        RepositoryService(locator_type locator, execution::IExecutor& executor,
            SharedPtrNotNull<db::ConnectionPool> connectionPool);

        auto FindAccountAsync(service::FindAccountParam param) -> Future<service::FindAccountResult> override;
        auto UpdateAccountAsync(service::UpdateAccountParam param) -> Future<service::UpdateAccountResult> override;

    private:
        auto SelectWorker(const std::string& account) const -> execution::IExecutor&;

        void LogDatabaseError(std::string_view diagnosticMessage);
        void LogException(std::string_view message);

        auto GetName() const -> std::string_view;

    private:
        locator_type _locator;
        SharedPtrNotNull<db::ConnectionPool> _connectionPool;

        SharedPtrNotNull<execution::IExecutor> _master;
        std::array<SharedPtrNotNull<execution::IExecutor>, 8> _worker;

        std::unordered_map<int64_t, std::shared_ptr<db::Account>> _accounts;
        std::unordered_map<std::string, std::shared_ptr<db::Account>> _accountNameIndexer;
    };
}
