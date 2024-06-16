
#include "database_service.h"

#include "database_service_message_json.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"

namespace zerosugar::xr::service
{
    DatabaseServiceProxy::DatabaseServiceProxy(SharedPtrNotNull<RPCClient> client)
        : _client(std::move(client))
    {
    }

    auto DatabaseServiceProxy::AddAccountAsync(AddAccountParam param) -> Future<AddAccountResult>
    {
        return _client->CallRemoteProcedure<AddAccountParam, Future<AddAccountResult>::value_type>(name, "AddAccountAsync", param);
    }

    auto DatabaseServiceProxy::GetAccountAsync(GetAccountParam param) -> Future<GetAccountResult>
    {
        return _client->CallRemoteProcedure<GetAccountParam, Future<GetAccountResult>::value_type>(name, "GetAccountAsync", param);
    }

    auto DatabaseServiceProxy::AddCharacterAsync(AddCharacterParam param) -> Future<AddCharacterResult>
    {
        return _client->CallRemoteProcedure<AddCharacterParam, Future<AddCharacterResult>::value_type>(name, "AddCharacterAsync", param);
    }

    auto DatabaseServiceProxy::GetLobbyCharactersAsync(GetLobbyCharactersParam param) -> Future<GetLobbyCharactersResult>
    {
        return _client->CallRemoteProcedure<GetLobbyCharactersParam, Future<GetLobbyCharactersResult>::value_type>(name, "GetLobbyCharactersAsync", param);
    }

    void Configure(const SharedPtrNotNull<IDatabaseService>& service, RPCClient& rpcClient)
    {
        rpcClient.RegisterProcedure("DatabaseService", "AddAccountAsync",
            [service = service](AddAccountParam param) -> Future<AddAccountResult>
            {
                return service->AddAccountAsync(std::move(param));
            });
        rpcClient.RegisterProcedure("DatabaseService", "GetAccountAsync",
            [service = service](GetAccountParam param) -> Future<GetAccountResult>
            {
                return service->GetAccountAsync(std::move(param));
            });
        rpcClient.RegisterProcedure("DatabaseService", "AddCharacterAsync",
            [service = service](AddCharacterParam param) -> Future<AddCharacterResult>
            {
                return service->AddCharacterAsync(std::move(param));
            });
        rpcClient.RegisterProcedure("DatabaseService", "GetLobbyCharactersAsync",
            [service = service](GetLobbyCharactersParam param) -> Future<GetLobbyCharactersResult>
            {
                return service->GetLobbyCharactersAsync(std::move(param));
            });
    }

}
