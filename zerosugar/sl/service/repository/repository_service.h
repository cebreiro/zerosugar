#pragma once
#include "zerosugar/sl/database/model/account.h"
#include "zerosugar/sl/service/generated/repository_service_generated_interface.h"

namespace zerosugar::execution
{
    class IExecutor;
}

namespace zerosugar::db
{
    class ConnectionPool;
}

namespace zerosugar::sl
{
    class RepositoryService final
        : public service::IRepositoryService
        , public std::enable_shared_from_this<RepositoryService>
    {
    public:
        using locator_type = ServiceLocatorT<ILogService>;

    public:
        RepositoryService(execution::IExecutor& executor, SharedPtrNotNull<zerosugar::db::ConnectionPool> connectionPool);

        void Initialize(ServiceLocator& serviceLocator) override;

        auto FindAccountAsync(service::FindAccountParam param) -> Future<service::FindAccountResult> override;
        auto UpdateAccountAsync(service::UpdateAccountParam param) -> Future<service::UpdateAccountResult> override;

        auto GetCharacterListAsync(service::GetCharacterListParam param) -> Future<service::GetCharacterListResult> override;
        auto CreateCharacterAsync(service::CreateCharacterParam param) -> Future<service::CreateCharacterResult> override;
        auto DeleteCharacterAsync(service::DeleteCharacterParam param) -> Future<service::DeleteCharacterResult> override;
        auto NameCheckCharacterAsync(service::NameCheckCharacterParam param) -> Future<service::NameCheckCharacterResult> override;

        auto LoadCharacterAsync(service::LoadCharacterParam param) -> Future<service::LoadCharacterResult> override;

    private:
        auto SelectWorker(const std::string& account) const -> execution::IExecutor&;
        auto SelectWorker(int64_t hash) const -> execution::IExecutor&;

        void LogDatabaseError(std::string_view diagnosticMessage);
        void LogException(std::string_view message);

        auto GetName() const -> std::string_view;

    private:
        locator_type _locator;
        SharedPtrNotNull<zerosugar::db::ConnectionPool> _connectionPool;

        SharedPtrNotNull<execution::IExecutor> _master;
        std::array<SharedPtrNotNull<execution::IExecutor>, 8> _worker;

        std::unordered_map<int64_t, std::shared_ptr<db::Account>> _accounts;
        std::unordered_map<std::string, std::shared_ptr<db::Account>> _accountNameIndexer;
    };
}
