#include "database_service.h"

#include "zerosugar/shared/execution/executor/executor_coroutine_traits.h"
#include "zerosugar/shared/database/connection/connection_pool.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"
#include "zerosugar/xr/service/model/generated/database_service_message_json.h"
#include "zerosugar/xr/service/database/stored_procedure/stored_procedure.hpp"

using namespace zerosugar::db;
using namespace zerosugar::xr::db;

namespace zerosugar::xr
{
    DatabaseService::DatabaseService(SharedPtrNotNull<execution::IExecutor> executor, SharedPtrNotNull<ConnectionPool> connectionPool)
        : _executor(std::move(executor))
        , _connectionPool(std::move(connectionPool))
    {
    }

    void DatabaseService::Initialize(ServiceLocator& serviceLocator)
    {
        IDatabaseService::Initialize(serviceLocator);

        _serviceLocator = serviceLocator;

        Configure(shared_from_this(), serviceLocator.Get<RPCClient>());
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
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        ConnectionPool::Borrowed conn = co_await _connectionPool->Pop();
        sp::AccountsAdd storedProcedure(conn, param.account, param.password);

        service::AddAccountResult result;
        result.errorCode = service::DatabaseServiceErrorCode::DatabaseErrorNone;

        DatabaseError error = co_await storedProcedure.ExecuteAsync();
        if (error)
        {
            result.errorCode = service::DatabaseServiceErrorCode::DatabaseErrorInternalError;

            LogError(__FUNCTION__, error);
        }

        co_return result;
    }

    auto DatabaseService::GetAccountAsync(service::GetAccountParam param) -> Future<service::GetAccountResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        ConnectionPool::Borrowed conn = co_await _connectionPool->Pop();
        sp::AccountsGet storedProcedure(conn, param.account);

        service::GetAccountResult result;
        result.errorCode = service::DatabaseServiceErrorCode::DatabaseErrorNone;

        DatabaseError error = co_await storedProcedure.ExecuteAsync();
        if (error)
        {
            result.errorCode = service::DatabaseServiceErrorCode::DatabaseErrorInternalError;

            LogError(__FUNCTION__, error);
        }
        else if (const std::optional<service::DTOAccount>& spResult = storedProcedure.GetResultAccount(); spResult.has_value())
        {
            result.account = *spResult;
        }
        else
        {
            result.errorCode = service::DatabaseServiceErrorCode::GetAccountErrorNotFound;
        }

        co_return result;
    }

    auto DatabaseService::AddCharacterAsync(service::AddCharacterParam param) -> Future<service::AddCharacterResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        ConnectionPool::Borrowed conn = co_await _connectionPool->Pop();
        sp::CharactersAdd storedProcedure(conn, param.characterAdd);

        service::AddCharacterResult result;
        result.errorCode = service::DatabaseServiceErrorCode::DatabaseErrorNone;

        DatabaseError error = co_await storedProcedure.ExecuteAsync();
        if (error)
        {
            result.errorCode = service::DatabaseServiceErrorCode::DatabaseErrorInternalError;

            LogError(__FUNCTION__, error);
        }
        else
        {
            result.addedCharacterId = storedProcedure.GetAddedCharacterId();
        }

        co_return result;
    }

    auto DatabaseService::GetLobbyCharactersAsync(service::GetLobbyCharactersParam param)
        -> Future<service::GetLobbyCharactersResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        ConnectionPool::Borrowed conn = co_await _connectionPool->Pop();
        sp::LobbyCharactersGetAll storedProcedure(conn, param.accountId);

        service::GetLobbyCharactersResult result;
        result.errorCode = service::DatabaseServiceErrorCode::DatabaseErrorNone;

        DatabaseError error = co_await storedProcedure.ExecuteAsync();
        if (error)
        {
            result.errorCode = service::DatabaseServiceErrorCode::DatabaseErrorInternalError;

            LogError(__FUNCTION__, error);
        }
        else
        {
            std::ranges::copy(storedProcedure.GetResult(), std::back_inserter(result.lobbyCharacters));
        }

        co_return result;
    }

    void DatabaseService::LogError(std::string_view function, const DatabaseError& error)
    {
        ZEROSUGAR_LOG_ERROR(_serviceLocator,
            std::format("[{}] {} query error. error: {}", name, function, error.What()));
    }
}
