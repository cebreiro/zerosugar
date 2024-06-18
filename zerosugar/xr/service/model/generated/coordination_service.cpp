
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

    auto CoordinationServiceProxy::OpenChannelAsync(AsyncEnumerable<CoordinationChannelInput> param) -> AsyncEnumerable<CoordinationChannelOutput>
    {
        return _client->CallRemoteProcedureClientServerStreaming<AsyncEnumerable<CoordinationChannelInput>::value_type, AsyncEnumerable<CoordinationChannelOutput>::value_type>(name, "OpenChannelAsync", std::move(param));
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

    void Configure(const SharedPtrNotNull<ICoordinationService>& service, RPCClient& rpcClient)
    {
        rpcClient.RegisterProcedure<false, false>("CoordinationService", "RegisterServerAsync",
            [service = service](RegisterServerParam param) -> Future<RegisterServerResult>
            {
                return service->RegisterServerAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<true, true>("CoordinationService", "OpenChannelAsync",
            [service = service](AsyncEnumerable<CoordinationChannelInput> param) -> AsyncEnumerable<CoordinationChannelOutput>
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
    }

}
