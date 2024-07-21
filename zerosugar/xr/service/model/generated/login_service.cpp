
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
        return _client->CallRemoteProcedure<LoginParam, Future<LoginResult>::value_type>(name, "LoginAsync", std::move(param));
    }

    auto LoginServiceProxy::CreateAccountAsync(CreateAccountParam param) -> Future<CreateAccountResult>
    {
        return _client->CallRemoteProcedure<CreateAccountParam, Future<CreateAccountResult>::value_type>(name, "CreateAccountAsync", std::move(param));
    }

    auto LoginServiceProxy::AuthenticateAsync(AuthenticateParam param) -> Future<AuthenticateResult>
    {
        return _client->CallRemoteProcedure<AuthenticateParam, Future<AuthenticateResult>::value_type>(name, "AuthenticateAsync", std::move(param));
    }

    auto LoginServiceProxy::RemoveAuthAsync(RemoveAuthParam param) -> Future<RemoveAuthResult>
    {
        return _client->CallRemoteProcedure<RemoveAuthParam, Future<RemoveAuthResult>::value_type>(name, "RemoveAuthAsync", std::move(param));
    }

    void Configure(const SharedPtrNotNull<ILoginService>& service, RPCClient& rpcClient)
    {
        rpcClient.RegisterProcedure<false, false>("LoginService", "LoginAsync",
            [service = service](LoginParam param) -> Future<LoginResult>
            {
                return service->LoginAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("LoginService", "CreateAccountAsync",
            [service = service](CreateAccountParam param) -> Future<CreateAccountResult>
            {
                return service->CreateAccountAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("LoginService", "AuthenticateAsync",
            [service = service](AuthenticateParam param) -> Future<AuthenticateResult>
            {
                return service->AuthenticateAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("LoginService", "RemoveAuthAsync",
            [service = service](RemoveAuthParam param) -> Future<RemoveAuthResult>
            {
                return service->RemoveAuthAsync(std::move(param));
            });
    }

}
