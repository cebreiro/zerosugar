#include "account_repository.h"

#include "zerosugar/shared/execution/future/future.hpp"
#include "zerosugar/sl/database/generated/account_table.h"

namespace zerosugar::sl
{
    AccountRepository::AccountRepository(boost::mysql::tcp_ssl_connection& connection)
        : _connection(connection)
    {
    }

    auto AccountRepository::Add(db::Account account) -> Future<bool>
    {
        db::AccountTable table(_connection);

        try
        {
            table.Add(account);
        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            // TODO: Log
            (void)e;
        }
        catch (const std::exception& e)
        {
            // TODO: Log
            (void)e;
        }
    }

    auto AccountRepository::Remove(int64_t accountId) -> Future<bool>
    {
        db::AccountTable table(_connection);

        try
        {
            table.Remove(accountId);
        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            // TODO: Log
            (void)e;
        }
        catch (const std::exception& e)
        {
            // TODO: Log
            (void)e;
        }
    }

    auto AccountRepository::Find(int64_t id) const -> Future<std::optional<db::Account>>
    {
        db::AccountTable table(_connection);

        try
        {
            co_return table.Find(id);
        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            // TODO: Log
            (void)e;
        }
        catch (const std::exception& e)
        {
            // TODO: Log
            (void)e;
        }
    }
}
