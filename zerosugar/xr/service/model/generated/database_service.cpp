
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
        return _client->CallRemoteProcedure<AddAccountParam, Future<AddAccountResult>::value_type>(name, "AddAccountAsync", std::move(param));
    }

    auto DatabaseServiceProxy::GetAccountAsync(GetAccountParam param) -> Future<GetAccountResult>
    {
        return _client->CallRemoteProcedure<GetAccountParam, Future<GetAccountResult>::value_type>(name, "GetAccountAsync", std::move(param));
    }

    auto DatabaseServiceProxy::AddCharacterAsync(AddCharacterParam param) -> Future<AddCharacterResult>
    {
        return _client->CallRemoteProcedure<AddCharacterParam, Future<AddCharacterResult>::value_type>(name, "AddCharacterAsync", std::move(param));
    }

    auto DatabaseServiceProxy::GetCharacterAsync(GetCharacterParam param) -> Future<GetCharacterResult>
    {
        return _client->CallRemoteProcedure<GetCharacterParam, Future<GetCharacterResult>::value_type>(name, "GetCharacterAsync", std::move(param));
    }

    auto DatabaseServiceProxy::RemoveCharacterAsync(RemoveCharacterParam param) -> Future<RemoveCharacterResult>
    {
        return _client->CallRemoteProcedure<RemoveCharacterParam, Future<RemoveCharacterResult>::value_type>(name, "RemoveCharacterAsync", std::move(param));
    }

    auto DatabaseServiceProxy::SaveCharacterItemChangeAsync(CharacterItemChangeParam param) -> Future<CharacterItemChangeResult>
    {
        return _client->CallRemoteProcedure<CharacterItemChangeParam, Future<CharacterItemChangeResult>::value_type>(name, "SaveCharacterItemChangeAsync", std::move(param));
    }

    auto DatabaseServiceProxy::GetLobbyCharactersAsync(GetLobbyCharactersParam param) -> Future<GetLobbyCharactersResult>
    {
        return _client->CallRemoteProcedure<GetLobbyCharactersParam, Future<GetLobbyCharactersResult>::value_type>(name, "GetLobbyCharactersAsync", std::move(param));
    }

    void Configure(const SharedPtrNotNull<IDatabaseService>& service, RPCClient& rpcClient)
    {
        rpcClient.RegisterProcedure<false, false>("DatabaseService", "AddAccountAsync",
            [service = service](AddAccountParam param) -> Future<AddAccountResult>
            {
                return service->AddAccountAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("DatabaseService", "GetAccountAsync",
            [service = service](GetAccountParam param) -> Future<GetAccountResult>
            {
                return service->GetAccountAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("DatabaseService", "AddCharacterAsync",
            [service = service](AddCharacterParam param) -> Future<AddCharacterResult>
            {
                return service->AddCharacterAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("DatabaseService", "GetCharacterAsync",
            [service = service](GetCharacterParam param) -> Future<GetCharacterResult>
            {
                return service->GetCharacterAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("DatabaseService", "RemoveCharacterAsync",
            [service = service](RemoveCharacterParam param) -> Future<RemoveCharacterResult>
            {
                return service->RemoveCharacterAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("DatabaseService", "SaveCharacterItemChangeAsync",
            [service = service](CharacterItemChangeParam param) -> Future<CharacterItemChangeResult>
            {
                return service->SaveCharacterItemChangeAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("DatabaseService", "GetLobbyCharactersAsync",
            [service = service](GetLobbyCharactersParam param) -> Future<GetLobbyCharactersResult>
            {
                return service->GetLobbyCharactersAsync(std::move(param));
            });
    }

}
