#include "login_service.h"

#include "zerosugar/xr/network/rpc/rpc_client.h"

namespace zerosugar::xr
{
    void LoginService::Initialize(ServiceLocator& dependencyLocator)
    {
        ILoginService::Initialize(dependencyLocator);

        ConfigureRemoteProcedureClient(dependencyLocator.Get<RPCClient>());
    }

    void LoginService::ConfigureRemoteProcedureClient(RPCClient& rpcClient)
    {
        // TODO: code-generation
        rpcClient.RegisterProcedure<LoginService>("LoginAsync",
            [self = shared_from_this()](service::LoginParam param) -> Future<service::LoginResult>
            {
                return self->LoginAsync(std::move(param));
            });

        rpcClient.RegisterProcedure<LoginService>("CreateAccountAsync",
            [self = shared_from_this()](service::CreateAccountParam param) -> Future<service::CreateAccountResult>
            {
                return self->CreateAccountAsync(std::move(param));
            });
    }
}
