#pragma once
#include <boost/mysql.hpp>
#include "zerosugar/sl/database/repository/account_repository_interface.h"

namespace zerosugar::db
{
    class ConnectionPool;
}

namespace zerosugar::execution
{
    class IExecutor;
}

namespace zerosugar::sl
{
    class AccountRepository : public IAccountRepository
    {
    public:
        AccountRepository() = delete;
        AccountRepository(zerosugar::db::ConnectionPool& connectionPool, SharedPtrNotNull<execution::IExecutor> executor);

        auto Add(db::Account account) -> Future<bool> override;
        auto Remove(int64_t accountId) -> Future<bool> override;
        auto Update(db::Account account) -> Future<bool> override;

        auto Find(int64_t id) const -> Future<std::optional<db::Account>> override;
        auto Find(std::string account) const -> Future<std::optional<db::Account>> override;

    private:
        zerosugar::db::ConnectionPool& _connectionPool;
        SharedPtrNotNull<execution::IExecutor> _executor;
    };
}
