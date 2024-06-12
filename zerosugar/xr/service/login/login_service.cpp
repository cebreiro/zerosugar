#include "login_service.h"

#include "zerosugar/xr/network/rpc/rpc_client.h"
#include "zerosugar/xr/service/model/generated/login_service_generated_json_serialize.h"

namespace zerosugar::xr
{
    LoginService::LoginService(SharedPtrNotNull<execution::IExecutor> executor)
        : _executor(std::move(executor))
        , _strand(std::make_shared<Strand>(_executor))
    {
    }

    void LoginService::Initialize(ServiceLocator& dependencyLocator)
    {
        ILoginService::Initialize(dependencyLocator);

        ConfigureRemoteProcedureClient(dependencyLocator.Get<RPCClient>());
    }

    void LoginService::Shutdown()
    {
        ILoginService::Shutdown();
    }

    void LoginService::Join(std::vector<boost::system::error_code>& errors)
    {
        ILoginService::Join(errors);
    }

    auto LoginService::LoginAsync(service::LoginParam param) -> Future<service::LoginResult>
    {
        (void)param;
        co_return{};
    }

    auto LoginService::CreateAccountAsync(service::CreateAccountParam param) -> Future<service::CreateAccountResult>
    {
        (void)param;
        co_return{};
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
