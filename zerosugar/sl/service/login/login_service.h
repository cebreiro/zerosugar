#pragma once
#include <boost/mp11.hpp>
#include "zerosugar/shared/service/service_locator.h"
#include "zerosugar/shared/log/log_service_interface.h"
#include "zerosugar/sl/service/generated/login_service_generated_interface.h"
#include "zerosugar/sl/service/generated/repository_service_generated_interface.h"
#include "zerosugar/sl/service/generated/world_service_generated_interface.h"
#include "zerosugar/sl/service/login/state/state.h"

namespace zerosugar
{
    class Strand;
}

namespace zerosugar::execution
{
    class IExecutor;
}

namespace zerosugar::sl
{
    class LoginService final
        : public service::ILoginService
        , public std::enable_shared_from_this<LoginService>
    {
    public:
        using Locator = ServiceLocatorRef<ILogService, service::IRepositoryService, service::IWorldService>;

    public:
        LoginService() = delete;
        explicit LoginService(execution::IExecutor& executor);

        void Initialize(ServiceLocator& dependencyLocator) override;
        void Shutdown() override;

        auto LoginAsync(service::LoginParam param) -> Future<service::LoginResult> override;
        auto LogoutAsync(service::LogoutParam param) -> Future<service::LogoutResult> override;

        auto AuthenticateAsync(service::AuthenticateParam param) -> Future<service::AuthenticateResult> override;
        auto AuthorizeAsync(service::AuthorizeParam param) -> Future<service::AuthorizeResult> override;

        auto KickAsync(service::KickParam param) -> Future<service::KickResult> override;

    private:
        void StartTokenExpireTimer(AuthTokenState& state, std::chrono::system_clock::time_point createTime, std::string token);
        void HandleTokenExpireTimer(std::string token, std::chrono::system_clock::time_point createTime);

        auto MakeAuthToken() -> std::string;
        static auto MakeExpireTimePoint() -> std::chrono::system_clock::time_point;

    private:
        auto GetRepositoryService() -> service::IRepositoryService&;

    private:
        std::atomic<bool> _shutdown = false;
        Locator _locator;
        SharedPtrNotNull<Strand> _strand;
        std::unordered_map<std::string, service::AuthToken> _loginAccounts;
        std::unordered_map<std::string, UniquePtrNotNull<AuthTokenState>> _states;

        std::mt19937 _mt;
    };
}
