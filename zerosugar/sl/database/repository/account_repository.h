#pragma once
#include <boost/mysql.hpp>
#include "zerosugar/sl/database/repository/account_repository_interface.h"

namespace zerosugar::sl
{
    class AccountRepository : public IAccountRepository
    {
    public:
        AccountRepository() = delete;

        explicit AccountRepository(boost::mysql::tcp_ssl_connection& connection);

        auto Add(db::Account account) -> Future<bool> override;
        auto Remove(int64_t accountId) -> Future<bool> override;
        auto Update(db::Account account) -> Future<bool> override;

        auto Find(int64_t id) const -> Future<std::optional<db::Account>> override;

    private:
        boost::mysql::tcp_ssl_connection& _connection;
    };
}
