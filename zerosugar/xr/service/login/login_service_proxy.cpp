#include "login_service_proxy.h"

#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"
#include "zerosugar/xr/service/model/generated/login_service_generated_json_serialize.h"

namespace zerosugar::xr
{
    LoginServiceProxy::LoginServiceProxy(SharedPtrNotNull<RPCClient> client)
        : _client(std::move(client))
    {
    }

    auto LoginServiceProxy::LoginAsync(service::LoginParam param) -> Future<service::LoginResult>
    {
        return _client->CallRemoteProcedure<ILoginService, service::LoginParam, service::LoginResult>("LoginAsync", param);
    }

    auto LoginServiceProxy::CreateAccountAsync(service::CreateAccountParam param)
        -> Future<service::CreateAccountResult>
    {
        return _client->CallRemoteProcedure<ILoginService, service::CreateAccountParam, service::CreateAccountResult>("CreateAccountAsync", param);
    }
}
