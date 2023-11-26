#include "account_repository.h"

#include "zerosugar/shared/execution/executor/executor_coroutine_traits.h"
#include "zerosugar/shared/execution/future/future.hpp"
#include "zerosugar/sl/database/connection/connection_pool.h"
#include "zerosugar/sl/database/generated/account_table.h"

namespace zerosugar::sl
{
    AccountRepository::AccountRepository(db::ConnectionPool& connectionPool,
        SharedPtrNotNull<execution::IExecutor> executor)
        : _connectionPool(connectionPool)
        , _executor(std::move(executor))
    {
    }

    auto AccountRepository::Add(db::Account account) -> Future<bool>
    {
        co_await *_executor;

        db::ConnectionPool::BorrowedConnection connection = co_await _connectionPool.Lend();
        assert(ExecutionContext::GetExecutor() == _executor.get());

        db::AccountTable table(*connection);

        try
        {
            table.Add(account);
            co_return true;
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

        co_return false;
    }

    auto AccountRepository::Remove(int64_t accountId) -> Future<bool>
    {
        co_await *_executor;

        db::ConnectionPool::BorrowedConnection connection = co_await _connectionPool.Lend();
        assert(ExecutionContext::GetExecutor() == _executor.get());

        db::AccountTable table(*connection);

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

        co_return false;
    }

    auto AccountRepository::Update(db::Account account) -> Future<bool>
    {
        co_await *_executor;

        db::ConnectionPool::BorrowedConnection connection = co_await _connectionPool.Lend();
        assert(ExecutionContext::GetExecutor() == _executor.get());

        db::AccountTable table(*connection);

        try
        {
            table.Update(account);
            co_return true;
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

        co_return false;
    }

    auto AccountRepository::Find(int64_t id) const -> Future<std::optional<db::Account>>
    {
        co_await *_executor;

        db::ConnectionPool::BorrowedConnection connection = co_await _connectionPool.Lend();
        assert(ExecutionContext::GetExecutor() == _executor.get());

        db::AccountTable table(*connection);

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

        co_return std::nullopt;
    }

    auto AccountRepository::Find(std::string account) const -> Future<std::optional<db::Account>>
    {
        co_await *_executor;

        db::ConnectionPool::BorrowedConnection connection = co_await _connectionPool.Lend();
        assert(ExecutionContext::GetExecutor() == _executor.get());

        db::AccountTable table(*connection);

        try
        {
            co_return table.FindByACCOUNT(account);
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

        co_return std::nullopt;
    }
}
