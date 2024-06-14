#include "database_service.h"

#include "zerosugar/shared/execution/executor/executor_coroutine_traits.h"
#include "zerosugar/shared/database/connection/connection_pool.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"
#include "zerosugar/xr/service/model/generated/database_service_message_json.h"

using namespace zerosugar::db;

namespace zerosugar::xr
{
    DatabaseService::DatabaseService(SharedPtrNotNull<execution::IExecutor> executor, SharedPtrNotNull<db::ConnectionPool> connectionPool)
        : _executor(std::move(executor))
        , _connectionPool(std::move(connectionPool))
    {
    }

    void DatabaseService::Initialize(ServiceLocator& serviceLocator)
    {
        IDatabaseService::Initialize(serviceLocator);

        _serviceLocator = serviceLocator;

        ConfigureRemoteProcedureClient(serviceLocator.Get<RPCClient>());
    }

    void DatabaseService::Shutdown()
    {
        IDatabaseService::Shutdown();
    }

    void DatabaseService::Join(std::vector<boost::system::error_code>& errors)
    {
        IDatabaseService::Join(errors);
    }

    auto DatabaseService::AddAccountAsync(service::AddAccountParam param) -> Future<service::AddAccountResult>
    {
        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        ConnectionPool::Borrowed conn = co_await _connectionPool->Pop();

        Promise<boost::mysql::error_code> promise;
        Future<boost::mysql::error_code> future = promise.GetFuture();

        boost::mysql::diagnostics dbDiagnostics;
        boost::mysql::results dbResult;

        service::AddAccountResult result;
        result.errorCode = service::DatabaseServiceErrorCode::DatabaseErrorNone;

        try
        {
            boost::mysql::statement stmt = conn->prepare_statement("CALL account_create(?, ?)");

            conn->async_execute(stmt.bind(param.account, param.password), dbResult, dbDiagnostics,
                [p = std::move(promise)](boost::mysql::error_code ec) mutable
                {
                    p.Set(std::move(ec));
                });

            boost::system::error_code ec = co_await future;
            assert(ExecutionContext::IsEqualTo(*_executor));

            throw_on_error(ec, dbDiagnostics);
        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            result.errorCode = service::DatabaseServiceErrorCode::DatabaseErrorInternalError;

            ZEROSUGAR_LOG_CRITICAL(_serviceLocator,
                std::format("[{}] AddAccountAsync query error. error: {}, diagnostics: {}",
                    name, e.what(), e.get_diagnostics().server_message().data()));
        }

        co_return result;
    }

    auto DatabaseService::GetAccountAsync(service::GetAccountParam param) -> Future<service::GetAccountResult>
    {
        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        ConnectionPool::Borrowed conn = co_await _connectionPool->Pop();

        Promise<boost::mysql::error_code> promise;
        Future<boost::mysql::error_code> future;

        boost::mysql::diagnostics dbDiagnostics;
        boost::mysql::results dbResult;

        boost::mysql::statement stmt = conn->prepare_statement("CALL account_get(?)");

        conn->async_execute(stmt.bind(param.account), dbResult, dbDiagnostics,
            [p = std::move(promise)](boost::mysql::error_code ec) mutable
            {
                p.Set(std::move(ec));
            });

        boost::system::error_code ec = co_await future;
        assert(ExecutionContext::IsEqualTo(*_executor));

        service::GetAccountResult result;
        result.errorCode = service::DatabaseServiceErrorCode::DatabaseErrorNone;

        try
        {
            throw_on_error(ec, dbDiagnostics);

            if (dbResult.rows().empty())
            {
                result.errorCode = service::DatabaseServiceErrorCode::GetAccountErrorNotFound;
            }
            else
            {
                const boost::mysql::row_view& front = dbResult.rows().front();

                result.accountId = front.at(0).as_int64();
                result.account = front.at(1).as_string();
                result.password = front.at(2).as_string();
                result.banned = static_cast<bool>(front.at(3).as_int64());
                result.deleted = static_cast<bool>(front.at(4).as_int64());
            }
        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            result.errorCode = service::DatabaseServiceErrorCode::DatabaseErrorInternalError;

            ZEROSUGAR_LOG_CRITICAL(_serviceLocator,
                std::format("[{}] AddAccountAsync query error. error: {}, diagnostics: {}",
                    name, e.what(), e.get_diagnostics().server_message().data()));
        }
        catch (const std::exception& e)
        {
            ZEROSUGAR_LOG_CRITICAL(_serviceLocator,
                std::format("[{}] AddAccountAsync query error. error: {}", name, e.what()));
        }

        co_return result;
    }

    void DatabaseService::ConfigureRemoteProcedureClient(RPCClient& rpcClient)
    {
        // TODO: code-generation
        rpcClient.RegisterProcedure<DatabaseService>("AddAccountAsync",
            [self = shared_from_this()](service::AddAccountParam param) -> Future<service::AddAccountResult>
            {
                return self->AddAccountAsync(std::move(param));
            });

        rpcClient.RegisterProcedure<DatabaseService>("GetAccountAsync",
            [self = shared_from_this()](service::GetAccountParam param) -> Future<service::GetAccountResult>
            {
                return self->GetAccountAsync(std::move(param));
            });
    }
}
