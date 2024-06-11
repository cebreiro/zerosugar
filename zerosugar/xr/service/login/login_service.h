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
        void Initialize(ServiceLocator& dependencyLocator) override;
        void Shutdown() override;
        void Join(std::vector<boost::system::error_code>& errors) override;

        auto LoginAsync(service::LoginParam param) -> Future<service::LoginResult> override;
        auto CreateAccountAsync(service::CreateAccountParam param) -> Future<service::CreateAccountResult> override;

    private:
        void RegisterRPC(RPCClient& rpcServer);
    };
}
