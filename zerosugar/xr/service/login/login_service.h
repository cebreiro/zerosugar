#pragma once
#include "zerosugar/xr/service/model/generated/login_service.h"
#include "zerosugar/xr/service/model/generated/database_service.h"

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
        ~LoginService();

        void Initialize(ServiceLocator& serviceLocator) override;
        void Shutdown() override;
        void Join(std::vector<boost::system::error_code>& errors) override;

        auto LoginAsync(service::LoginParam param) -> Future<service::LoginResult> override;
        auto CreateAccountAsync(service::CreateAccountParam param) -> Future<service::CreateAccountResult> override;
        auto AuthenticateAsync(service::AuthenticateParam param) -> Future<service::AuthenticateResult> override;

    private:
        auto Encode(const std::string& str) -> std::string;

    private:
        SharedPtrNotNull<execution::IExecutor> _executor;
        SharedPtrNotNull<Strand> _strand;
        void* evpContext = nullptr;

        ServiceLocatorT<service::IDatabaseService> _serviceLocator;

        std::mt19937 _mt;
        std::unordered_map<std::string, int64_t> _authenticationTokens;
    };
}
