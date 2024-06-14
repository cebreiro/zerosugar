
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
}
