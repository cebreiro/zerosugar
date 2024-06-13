#pragma once
#include "zerosugar/xr/service/model/generated/login_service_generated_interface.h"

namespace zerosugar::xr
{
    class RPCClient;

    class LoginService final
        : public service::ILoginService
        , public std::enable_shared_from_this<LoginService>
    {
    public:
        LoginService() = delete;

        explicit LoginService(SharedPtrNotNull<execution::IExecutor> executor);

        void Initialize(ServiceLocator& serviceLocator) override;
        void Shutdown() override;
        void Join(std::vector<boost::system::error_code>& errors) override;

        auto LoginAsync(service::LoginParam param) -> Future<service::LoginResult> override;
        auto CreateAccountAsync(service::CreateAccountParam param) -> Future<service::CreateAccountResult> override;

    private:
        void ConfigureRemoteProcedureClient(RPCClient& rpcClient);

    private:
        SharedPtrNotNull<execution::IExecutor> _executor;
        SharedPtrNotNull<Strand> _strand;
    };
}
