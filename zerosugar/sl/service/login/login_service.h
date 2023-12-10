#pragma once
#include <random>
#include <unordered_map>
#include <unordered_set>
#include "zerosugar/shared/service/service_locator.h"
#include "zerosugar/shared/log/log_service_interface.h"
#include "zerosugar/sl/service/generated/login_service_generated_interface.h"
#include "zerosugar/sl/service/generated/repository_service_generated_interface.h"
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
        using Locator = ServiceLocatorRef<ILogService, service::IRepositoryService>;

    public:
        LoginService() = delete;
        LoginService(Locator locator, execution::IExecutor& executor);

        void Shutdown() override;

        auto LoginAsync(service::LoginParam param) -> Future<service::LoginResult> override;
        auto LogoutAsync(service::LogoutParam param) -> Future<service::LogoutResult> override;

    private:
        void StartTokenExpireTimer(const service::AuthToken& token, const AuthTokenStatus& status);
        void HandleTokenExpireTimer(const service::AuthToken& token);

        auto MakeAuthToken() -> service::AuthToken;
        static auto MakeExpireTimePoint() -> std::chrono::system_clock::time_point;

    private:
        auto GetRepositoryService() -> service::IRepositoryService&;

    private:
        std::atomic<bool> _shutdown = false;
        Locator _locator;
        SharedPtrNotNull<Strand> _strand;
        std::unordered_set<std::string> _loginAccounts;
        std::unordered_map<std::string, AuthTokenStatus> _statuses;

        std::mt19937 _mt;
    };
}
