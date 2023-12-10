#include "repository_service.h"

#include "zerosugar/sl/database/connection/connection_pool.h"
#include "zerosugar/sl/database/generated/account_table.h"

namespace zerosugar::sl
{
    auto Convert(const db::Account& account) -> service::Account
    {
        return service::Account{
            .id = account.id,
            .account = account.account,
            .password = account.password,
            .gmLevel = account.gm_level,
            .banned = static_cast<bool>(account.banned),
            .banReason = account.ban_reason ? *account.ban_reason : std::string(),
        };
    }

    RepositoryService::RepositoryService(locator_type locator, execution::IExecutor& executor,
        SharedPtrNotNull<db::ConnectionPool> connectionPool)
        : _locator(std::move(locator))
        , _connectionPool(std::move(connectionPool))
    {
        auto e = executor.SharedFromThis();

        _master = std::make_shared<Strand>(e);

        for (int64_t i = 0; i < std::ssize(_worker); ++i)
        {
            _worker[i] = std::make_shared<Strand>(e);
        }
    }

    auto RepositoryService::FindAccountAsync(service::FindAccountParam param) -> Future<service::FindAccountResult>
    {
        co_await *_master;
        assert(ExecutionContext::GetExecutor() == _master.get());

        if (auto iter = _accountNameIndexer.find(param.account); iter != _accountNameIndexer.end())
        {
            co_return service::FindAccountResult{
                .account = Convert(*iter->second),
            };
        }

        execution::IExecutor& executor = SelectWorker(param.account);
        co_await executor;
        assert(ExecutionContext::GetExecutor() == &executor);

        std::optional<db::Account> account = std::nullopt;
        try
        {
            db::ConnectionPool::BorrowedConnection connection = co_await _connectionPool->Lend();
            assert(ExecutionContext::GetExecutor() == &executor);

            assert(connection.IsValid());

            db::AccountTable table(*connection);
            account =  table.FindByACCOUNT(param.account);
        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            LogDatabaseError(e.get_diagnostics().server_message());
            
        }
        catch (const std::exception& e)
        {
            LogDatabaseError(e.what());
        }

        if (!account.has_value())
        {
            co_return service::FindAccountResult{
                .errorCode = service::RepositoryServiceErrorCode::RepositoryAccountFailToFindKey,
            };
        }

        co_await *_master;
        assert(ExecutionContext::GetExecutor() == _master.get());

        auto accountCache = std::make_shared<db::Account>(*account);
        _accounts[account->id] = accountCache;
        _accountNameIndexer[account->account] = std::move(accountCache);

        co_return service::FindAccountResult{
            .account = Convert(*account),
        };
    }

    auto RepositoryService::UpdateAccountAsync(
        service::UpdateAccountParam param) -> Future<service::UpdateAccountResult>
    {
        co_await *_master;
        assert(ExecutionContext::GetExecutor() == _master.get());

        auto iter = _accounts.find(param.accountUpdate.id);
        if (iter == _accounts.end())
        {
            co_return service::UpdateAccountResult{
                .errorCode = service::RepositoryServiceErrorCode::RepositoryAccountFailToFindKey,
            };
        }

        std::shared_ptr<db::Account>& oldOne = iter->second;
        db::Account newOne = *oldOne;
        {
            if (param.accountUpdate.password.has_value())
            {
                newOne.password = *param.accountUpdate.password;
            }

            if (param.accountUpdate.gmLevel.has_value())
            {
                newOne.gm_level = static_cast<int8_t>(*param.accountUpdate.gmLevel);
            }

            if (param.accountUpdate.banned.has_value())
            {
                newOne.banned = *param.accountUpdate.banned;
            }

            if (param.accountUpdate.banReason.has_value())
            {
                newOne.ban_reason = *param.accountUpdate.banReason;
            }
        }

        execution::IExecutor& executor = SelectWorker(oldOne->account);
        co_await executor;
        assert(ExecutionContext::GetExecutor() == &executor);

        bool success = false;
        try
        {
            db::ConnectionPool::BorrowedConnection connection = co_await _connectionPool->Lend();
            assert(ExecutionContext::GetExecutor() == &executor);

            assert(connection.IsValid());

            db::AccountTable table(*connection);
            table.UpdateDifference(*oldOne, newOne);

            success = true;
        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            LogDatabaseError(e.get_diagnostics().server_message());
        }
        catch (const std::exception& e)
        {
            LogDatabaseError(e.what());
        }

        if (!success)
        {
            co_return service::UpdateAccountResult{
                .errorCode = service::RepositoryServiceErrorCode::RepositoryInternalDbError,
            };
        }

        co_await *_master;
        assert(ExecutionContext::GetExecutor() == _master.get());

        *oldOne = newOne;

        co_return service::UpdateAccountResult{
            .errorCode = service::RepositoryServiceErrorCode::RepositoryErrorNone,
        };
    }

    void RepositoryService::LogDatabaseError(std::string_view diagnosticMessage)
    {
        ZEROSUGAR_LOG_ERROR(_locator, std::format("[{}] db error. error: {}",
            GetName(), diagnosticMessage));
    }

    void RepositoryService::LogException(std::string_view message)
    {
        ZEROSUGAR_LOG_ERROR(_locator, std::format("[{}] throws an exception. exception: {}",
            GetName(), message));
    }

    auto RepositoryService::GetName() const -> std::string_view
    {
        return "repository_service";
    }

    auto RepositoryService::SelectWorker(const std::string& account) const -> execution::IExecutor&
    {
        return *_worker[std::hash<std::string>()(account) % std::ssize(_worker)];
    }
}
