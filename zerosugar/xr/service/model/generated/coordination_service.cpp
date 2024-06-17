
#include "coordination_service.h"

#include "coordination_service_message_json.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"

namespace zerosugar::xr::service
{
    CoordinationServiceProxy::CoordinationServiceProxy(SharedPtrNotNull<RPCClient> client)
        : _client(std::move(client))
    {
    }

    auto CoordinationServiceProxy::GetNameAsync(GetNameParam param) -> Future<GetNameResult>
    {
        return _client->CallRemoteProcedure<GetNameParam, Future<GetNameResult>::value_type>(name, "GetNameAsync", param);
    }

    auto CoordinationServiceProxy::RequestSnowflakeKeyAsync(RequestSnowflakeKeyParam param) -> Future<RequestSnowflakeKeyResult>
    {
        return _client->CallRemoteProcedure<RequestSnowflakeKeyParam, Future<RequestSnowflakeKeyResult>::value_type>(name, "RequestSnowflakeKeyAsync", param);
    }

    auto CoordinationServiceProxy::ReturnSnowflakeKeyAsync(ReturnSnowflakeKeyParam param) -> Future<ReturnSnowflakeKeyResult>
    {
        return _client->CallRemoteProcedure<ReturnSnowflakeKeyParam, Future<ReturnSnowflakeKeyResult>::value_type>(name, "ReturnSnowflakeKeyAsync", param);
    }

    auto CoordinationServiceProxy::AddPlayerAsync(AddPlayerParam param) -> Future<AddPlayerResult>
    {
        return _client->CallRemoteProcedure<AddPlayerParam, Future<AddPlayerResult>::value_type>(name, "AddPlayerAsync", param);
    }

    void Configure(const SharedPtrNotNull<ICoordinationService>& service, RPCClient& rpcClient)
    {
        rpcClient.RegisterProcedure("CoordinationService", "GetNameAsync",
            [service = service](GetNameParam param) -> Future<GetNameResult>
            {
                return service->GetNameAsync(std::move(param));
            });
        rpcClient.RegisterProcedure("CoordinationService", "RequestSnowflakeKeyAsync",
            [service = service](RequestSnowflakeKeyParam param) -> Future<RequestSnowflakeKeyResult>
            {
                return service->RequestSnowflakeKeyAsync(std::move(param));
            });
        rpcClient.RegisterProcedure("CoordinationService", "ReturnSnowflakeKeyAsync",
            [service = service](ReturnSnowflakeKeyParam param) -> Future<ReturnSnowflakeKeyResult>
            {
                return service->ReturnSnowflakeKeyAsync(std::move(param));
            });
        rpcClient.RegisterProcedure("CoordinationService", "AddPlayerAsync",
            [service = service](AddPlayerParam param) -> Future<AddPlayerResult>
            {
                return service->AddPlayerAsync(std::move(param));
            });
    }

}
