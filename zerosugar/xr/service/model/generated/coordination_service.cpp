
#include "coordination_service.h"

#include "coordination_service_message_json.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"

namespace zerosugar::xr::service
{
    CoordinationServiceProxy::CoordinationServiceProxy(SharedPtrNotNull<RPCClient> client)
        : _client(std::move(client))
    {
    }

    auto CoordinationServiceProxy::RegisterServerAsync(RegisterServerParam param) -> Future<RegisterServerResult>
    {
        return _client->CallRemoteProcedure<RegisterServerParam, Future<RegisterServerResult>::value_type>(name, "RegisterServerAsync", std::move(param));
    }

    auto CoordinationServiceProxy::UpdateServerStatusAsync(UpdateServerStatusParam param) -> Future<UpdateServerStatusResult>
    {
        return _client->CallRemoteProcedure<UpdateServerStatusParam, Future<UpdateServerStatusResult>::value_type>(name, "UpdateServerStatusAsync", std::move(param));
    }

    auto CoordinationServiceProxy::OpenChannelAsync(AsyncEnumerable<CoordinationCommandResponse> param) -> AsyncEnumerable<CoordinationCommand>
    {
        return _client->CallRemoteProcedureClientServerStreaming<AsyncEnumerable<CoordinationCommandResponse>::value_type, AsyncEnumerable<CoordinationCommand>::value_type>(name, "OpenChannelAsync", std::move(param));
    }

    auto CoordinationServiceProxy::RequestSnowflakeKeyAsync(RequestSnowflakeKeyParam param) -> Future<RequestSnowflakeKeyResult>
    {
        return _client->CallRemoteProcedure<RequestSnowflakeKeyParam, Future<RequestSnowflakeKeyResult>::value_type>(name, "RequestSnowflakeKeyAsync", std::move(param));
    }

    auto CoordinationServiceProxy::ReturnSnowflakeKeyAsync(ReturnSnowflakeKeyParam param) -> Future<ReturnSnowflakeKeyResult>
    {
        return _client->CallRemoteProcedure<ReturnSnowflakeKeyParam, Future<ReturnSnowflakeKeyResult>::value_type>(name, "ReturnSnowflakeKeyAsync", std::move(param));
    }

    auto CoordinationServiceProxy::AddPlayerAsync(AddPlayerParam param) -> Future<AddPlayerResult>
    {
        return _client->CallRemoteProcedure<AddPlayerParam, Future<AddPlayerResult>::value_type>(name, "AddPlayerAsync", std::move(param));
    }

    auto CoordinationServiceProxy::RemovePlayerAsync(RemovePlayerParam param) -> Future<RemovePlayerResult>
    {
        return _client->CallRemoteProcedure<RemovePlayerParam, Future<RemovePlayerResult>::value_type>(name, "RemovePlayerAsync", std::move(param));
    }

    auto CoordinationServiceProxy::AuthenticatePlayerAsync(AuthenticatePlayerParam param) -> Future<AuthenticatePlayerResult>
    {
        return _client->CallRemoteProcedure<AuthenticatePlayerParam, Future<AuthenticatePlayerResult>::value_type>(name, "AuthenticatePlayerAsync", std::move(param));
    }

    auto CoordinationServiceProxy::RemoveGameInstanceAsync(RemoveGameInstanceParam param) -> Future<RemoveGameInstanceResult>
    {
        return _client->CallRemoteProcedure<RemoveGameInstanceParam, Future<RemoveGameInstanceResult>::value_type>(name, "RemoveGameInstanceAsync", std::move(param));
    }

    auto CoordinationServiceProxy::BroadcastChattingAsync(BroadcastChattingParam param) -> Future<BroadcastChattingResult>
    {
        return _client->CallRemoteProcedure<BroadcastChattingParam, Future<BroadcastChattingResult>::value_type>(name, "BroadcastChattingAsync", std::move(param));
    }

    auto CoordinationServiceProxy::RequestDungeonMatchAsync(RequestDungeonMatchParam param) -> Future<RequestDungeonMatchResult>
    {
        return _client->CallRemoteProcedure<RequestDungeonMatchParam, Future<RequestDungeonMatchResult>::value_type>(name, "RequestDungeonMatchAsync", std::move(param));
    }

    auto CoordinationServiceProxy::CancelDungeonMatchAsync(CancelDungeonMatchParam param) -> Future<CancelDungeonMatchResult>
    {
        return _client->CallRemoteProcedure<CancelDungeonMatchParam, Future<CancelDungeonMatchResult>::value_type>(name, "CancelDungeonMatchAsync", std::move(param));
    }

    auto CoordinationServiceProxy::ApproveDungeonMatchAsync(ApproveDungeonMatchParam param) -> Future<ApproveDungeonMatchResult>
    {
        return _client->CallRemoteProcedure<ApproveDungeonMatchParam, Future<ApproveDungeonMatchResult>::value_type>(name, "ApproveDungeonMatchAsync", std::move(param));
    }

    auto CoordinationServiceProxy::RejectDungeonMatchAsync(RejectDungeonMatchParam param) -> Future<RejectDungeonMatchResult>
    {
        return _client->CallRemoteProcedure<RejectDungeonMatchParam, Future<RejectDungeonMatchResult>::value_type>(name, "RejectDungeonMatchAsync", std::move(param));
    }

    void Configure(const SharedPtrNotNull<ICoordinationService>& service, RPCClient& rpcClient)
    {
        rpcClient.RegisterProcedure<false, false>("CoordinationService", "RegisterServerAsync",
            [service = service](RegisterServerParam param) -> Future<RegisterServerResult>
            {
                return service->RegisterServerAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("CoordinationService", "UpdateServerStatusAsync",
            [service = service](UpdateServerStatusParam param) -> Future<UpdateServerStatusResult>
            {
                return service->UpdateServerStatusAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<true, true>("CoordinationService", "OpenChannelAsync",
            [service = service](AsyncEnumerable<CoordinationCommandResponse> param) -> AsyncEnumerable<CoordinationCommand>
            {
                return service->OpenChannelAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("CoordinationService", "RequestSnowflakeKeyAsync",
            [service = service](RequestSnowflakeKeyParam param) -> Future<RequestSnowflakeKeyResult>
            {
                return service->RequestSnowflakeKeyAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("CoordinationService", "ReturnSnowflakeKeyAsync",
            [service = service](ReturnSnowflakeKeyParam param) -> Future<ReturnSnowflakeKeyResult>
            {
                return service->ReturnSnowflakeKeyAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("CoordinationService", "AddPlayerAsync",
            [service = service](AddPlayerParam param) -> Future<AddPlayerResult>
            {
                return service->AddPlayerAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("CoordinationService", "RemovePlayerAsync",
            [service = service](RemovePlayerParam param) -> Future<RemovePlayerResult>
            {
                return service->RemovePlayerAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("CoordinationService", "AuthenticatePlayerAsync",
            [service = service](AuthenticatePlayerParam param) -> Future<AuthenticatePlayerResult>
            {
                return service->AuthenticatePlayerAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("CoordinationService", "RemoveGameInstanceAsync",
            [service = service](RemoveGameInstanceParam param) -> Future<RemoveGameInstanceResult>
            {
                return service->RemoveGameInstanceAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("CoordinationService", "BroadcastChattingAsync",
            [service = service](BroadcastChattingParam param) -> Future<BroadcastChattingResult>
            {
                return service->BroadcastChattingAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("CoordinationService", "RequestDungeonMatchAsync",
            [service = service](RequestDungeonMatchParam param) -> Future<RequestDungeonMatchResult>
            {
                return service->RequestDungeonMatchAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("CoordinationService", "CancelDungeonMatchAsync",
            [service = service](CancelDungeonMatchParam param) -> Future<CancelDungeonMatchResult>
            {
                return service->CancelDungeonMatchAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("CoordinationService", "ApproveDungeonMatchAsync",
            [service = service](ApproveDungeonMatchParam param) -> Future<ApproveDungeonMatchResult>
            {
                return service->ApproveDungeonMatchAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("CoordinationService", "RejectDungeonMatchAsync",
            [service = service](RejectDungeonMatchParam param) -> Future<RejectDungeonMatchResult>
            {
                return service->RejectDungeonMatchAsync(std::move(param));
            });
    }

}
