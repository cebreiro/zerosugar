
#include "gateway_service.h"

#include "gateway_service_message_json.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"

namespace zerosugar::xr::service
{
    GatewayServiceProxy::GatewayServiceProxy(SharedPtrNotNull<RPCClient> client)
        : _client(std::move(client))
    {
    }

    auto GatewayServiceProxy::AddGameServiceAsync(AddGameServiceParam param) -> Future<AddGameServiceResult>
    {
        return _client->CallRemoteProcedure<AddGameServiceParam, Future<AddGameServiceResult>::value_type>(name, "AddGameServiceAsync", std::move(param));
    }

    auto GatewayServiceProxy::GetGameServiceListAsync(GetGameServiceListParam param) -> Future<GetGameServiceListResult>
    {
        return _client->CallRemoteProcedure<GetGameServiceListParam, Future<GetGameServiceListResult>::value_type>(name, "GetGameServiceListAsync", std::move(param));
    }

    void Configure(const SharedPtrNotNull<IGatewayService>& service, RPCClient& rpcClient)
    {
        rpcClient.RegisterProcedure<false, false>("GatewayService", "AddGameServiceAsync",
            [service = service](AddGameServiceParam param) -> Future<AddGameServiceResult>
            {
                return service->AddGameServiceAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("GatewayService", "GetGameServiceListAsync",
            [service = service](GetGameServiceListParam param) -> Future<GetGameServiceListResult>
            {
                return service->GetGameServiceListAsync(std::move(param));
            });
    }

}
