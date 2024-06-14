
#include "login_service.h"

#include "login_service_message_json.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"

namespace zerosugar::xr::service
{
    LoginServiceProxy::LoginServiceProxy(SharedPtrNotNull<RPCClient> client)
        : _client(std::move(client))
    {
    }
    auto LoginServiceProxy::LoginAsync(LoginParam param) -> Future<LoginResult>
    {
        return _client->CallRemoteProcedure<LoginParam, Future<LoginResult>::value_type>(name, "LoginAsync", param);
    }
    auto LoginServiceProxy::CreateAccountAsync(CreateAccountParam param) -> Future<CreateAccountResult>
    {
        return _client->CallRemoteProcedure<CreateAccountParam, Future<CreateAccountResult>::value_type>(name, "CreateAccountAsync", param);
    }
}
