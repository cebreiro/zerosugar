#include "login_service.h"

#include "zerosugar/xr/network/rpc_client.h"

namespace zerosugar::xr
{
    void LoginService::Initialize(ServiceLocator& dependencyLocator)
    {
        ILoginService::Initialize(dependencyLocator);


        RPCClient& client = dependencyLocator.Get<RPCClient>();
        RegisterRPC(client);
    }

    void LoginService::RegisterRPC(RPCClient& client)
    {
        // TODO: code-generation
        client.RegisterProcedure("LoginAsync",
            [self = shared_from_this()](service::LoginParam param) -> Future<service::LoginResult>
            {
                return self->LoginAsync(std::move(param));
            });

        client.RegisterProcedure("CreateAccountAsync",
            [self = shared_from_this()](service::CreateAccountParam param) -> Future<service::CreateAccountResult>
            {
                return self->CreateAccountAsync(std::move(param));
            });
    }
}
