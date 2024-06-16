
#include "game_service.h"

#include "game_service_message_json.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"

namespace zerosugar::xr::service
{
    GameServiceProxy::GameServiceProxy(SharedPtrNotNull<RPCClient> client)
        : _client(std::move(client))
    {
    }

    auto GameServiceProxy::GetNameAsync(GetNameParam param) -> Future<GetNameResult>
    {
        return _client->CallRemoteProcedure<GetNameParam, Future<GetNameResult>::value_type>(name, "GetNameAsync", param);
    }

    auto GameServiceProxy::RequestSnowflakeKeyAsync(RequestSnowflakeKeyParam param) -> Future<RequestSnowflakeKeyResult>
    {
        return _client->CallRemoteProcedure<RequestSnowflakeKeyParam, Future<RequestSnowflakeKeyResult>::value_type>(name, "RequestSnowflakeKeyAsync", param);
    }

    auto GameServiceProxy::ReturnSnowflakeKeyAsync(ReturnSnowflakeKeyParam param) -> Future<ReturnSnowflakeKeyResult>
    {
        return _client->CallRemoteProcedure<ReturnSnowflakeKeyParam, Future<ReturnSnowflakeKeyResult>::value_type>(name, "ReturnSnowflakeKeyAsync", param);
    }

    void Configure(const SharedPtrNotNull<IGameService>& service, RPCClient& rpcClient)
    {
        rpcClient.RegisterProcedure("GameService", "GetNameAsync",
            [service = service](GetNameParam param) -> Future<GetNameResult>
            {
                return service->GetNameAsync(std::move(param));
            });
        rpcClient.RegisterProcedure("GameService", "RequestSnowflakeKeyAsync",
            [service = service](RequestSnowflakeKeyParam param) -> Future<RequestSnowflakeKeyResult>
            {
                return service->RequestSnowflakeKeyAsync(std::move(param));
            });
        rpcClient.RegisterProcedure("GameService", "ReturnSnowflakeKeyAsync",
            [service = service](ReturnSnowflakeKeyParam param) -> Future<ReturnSnowflakeKeyResult>
            {
                return service->ReturnSnowflakeKeyAsync(std::move(param));
            });
    }

}
