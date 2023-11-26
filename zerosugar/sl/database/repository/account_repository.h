#pragma once
#include <boost/mysql.hpp>
#include "zerosugar/sl/database/repository/account_repository_interface.h"

namespace zerosugar::execution
{
    class IExecutor;
}

namespace zerosugar::sl
{
    namespace db
    {
        class ConnectionPool;
    }

    class AccountRepository : public IAccountRepository
    {
    public:
        AccountRepository() = delete;
        AccountRepository(db::ConnectionPool& connectionPool, SharedPtrNotNull<execution::IExecutor> executor);

        auto Add(db::Account account) -> Future<bool> override;
        auto Remove(int64_t accountId) -> Future<bool> override;
        auto Update(db::Account account) -> Future<bool> override;

        auto Find(int64_t id) const -> Future<std::optional<db::Account>> override;
        auto Find(std::string account) const -> Future<std::optional<db::Account>> override;

    private:
        db::ConnectionPool& _connectionPool;
        SharedPtrNotNull<execution::IExecutor> _executor;
    };
}
