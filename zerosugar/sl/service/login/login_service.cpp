#include "login_service.h"

#include "zerosugar/shared/service/service_locator_log.h"
#include "zerosugar/shared/execution/future/future_coroutine_traits.h"
#include "zerosugar/shared/execution/executor/strand.h"
#include "zerosugar/shared/execution/executor/executor_coroutine_traits.h"
#include "zerosugar/shared/execution/future/operation/delay.h"

namespace zerosugar::sl
{
    using namespace service;

    LoginService::LoginService(execution::IExecutor& executor)
        : _strand(std::make_shared<Strand>(executor.SharedFromThis()))
        , _mt(std::random_device()())
    {
    }

    void LoginService::Initialize(ServiceLocator& serviceLocator)
    {
        _locator = serviceLocator;
        if (!_locator.ContainsAll())
        {
            throw std::runtime_error("[sl_login_service] dependency is not satisfied");
        }
    }

    void LoginService::Shutdown()
    {
        _shutdown.store(true);
    }

    auto LoginService::LoginAsync(LoginParam param) -> Future<LoginResult>
    {
        co_await *_strand;
        assert(ExecutionContext::GetExecutor() == _strand.get());

        const FindAccountResult findAccountResult = co_await GetRepositoryService().FindAccountAsync(FindAccountParam{
            .account = param.account,
            });
        assert(ExecutionContext::GetExecutor() == _strand.get());

        const std::optional<Account>& account = findAccountResult.account;

        if (!account.has_value() || findAccountResult.errorCode != RepositoryServiceErrorCode::RepositoryErrorNone)
        {
            ZEROSUGAR_LOG_INFO(_locator, std::format("[sl_login_service] login fail. account not found. requester: {}, account: {}",
                param.context, param.account));

            co_return LoginResult{
                .errorCode = LoginServiceErrorCode::LoginErrorFailInvalid,
            };
        }

        if (account->password != param.password)
        {
            ZEROSUGAR_LOG_INFO(_locator, std::format("[sl_login_service] login fail. invalid password. requester: {}, account: {}",
                param.context, param.account));

            co_return LoginResult{
                .errorCode = LoginServiceErrorCode::LoginErrorFailInvalid,
            };
        }

        if (account->banned)
        {
            ZEROSUGAR_LOG_INFO(_locator, std::format("[sl_login_service] login fail. account is not banned. requester: {}, account: {}",
                param.context, param.account));

            co_return LoginResult{
                .errorCode = LoginServiceErrorCode::LoginErrorFailInvalid,
            };
        }

        const auto& [loginAccountIter, insertedLoginAccount] = _loginAccounts.try_emplace(param.account, AuthToken{});
        if (!insertedLoginAccount)
        {
            ZEROSUGAR_LOG_INFO(_locator, std::format("[sl_login_service] login fail. account already exists. context: {}, account: {}",
                param.context, param.account));

            co_return LoginResult{
                .errorCode = LoginServiceErrorCode::LoginErrorFailLoginDuplicate,
            };
        }

        std::string token;
        auto* state = new AuthTokenState(account->id, account->account, param.address, MakeExpireTimePoint());
        do
        {
            const auto& [iter, inserted] = _states.try_emplace(
                MakeAuthToken(), std::unique_ptr<AuthTokenState>());
            if (inserted)
            {
                token = iter->first;
                iter->second = std::unique_ptr<AuthTokenState>(state);

                break;
            }
        } while (true);

        ZEROSUGAR_LOG_INFO(_locator, std::format("[sl_login_service] token generated. account: {}, token: {}",
            param.account, token));

        loginAccountIter->second.values = token;
        StartTokenExpireTimer(*state, state->GetCreateTime(), token);

        co_return LoginResult{
            .errorCode = LoginServiceErrorCode::LoginErrorNone,
            .token = AuthToken{ .values = std::move(token) },
            .accountId = account->id,
        };
    }

    auto LoginService::LogoutAsync(LogoutParam param) -> Future<LogoutResult>
    {
        co_await *_strand;
        assert(ExecutionContext::GetExecutor() == _strand.get());

        auto iter = _states.find(param.token.values);
        if (iter == _states.end())
        {
            // TODO: add virtual auto IService::GetName() const = 0 and use it
            ZEROSUGAR_LOG_INFO(_locator, std::format("[sl_login_service] login fail. auth_token is not found. context: {}",
                param.context));

            co_return LogoutResult{
                .errorCode = LoginServiceErrorCode::LoginErrorFailInvalid,
            };
        }

        const std::string& account = iter->second->GetAccount();
        ZEROSUGAR_LOG_INFO(_locator, std::format("[sl_login_service] logout. account: {}, context: {}",
            account, param.context));

        _loginAccounts.erase(account);
        _states.erase(iter);

        co_return LogoutResult{
             .errorCode = LoginServiceErrorCode::LoginErrorNone,
        };
    }

    auto LoginService::AuthenticateAsync(AuthenticateParam param) -> Future<AuthenticateResult>
    {
        co_await *_strand;
        assert(ExecutionContext::GetExecutor() == _strand.get());

        auto iter = _states.find(param.token.values);
        if (iter == _states.end() || iter->second->GetAddress() != param.address)
        {
            ZEROSUGAR_LOG_INFO(_locator, std::format("[sl_login_service] fail to authenticate reqeust. address: {}",
                param.address));

            co_return AuthenticateResult{
                .errorCode = LoginServiceErrorCode::LoginErrorFailAuthenticateFailure,
            };
        }

        co_return AuthenticateResult{
            .errorCode = LoginServiceErrorCode::LoginErrorNone,
            .accountId = iter->second->GetAccountId(),
        };
    }

    auto LoginService::AuthorizeAsync(AuthorizeParam param) -> Future<AuthorizeResult>
    {
        (void)param;

        co_return AuthorizeResult{
            .errorCode = LoginServiceErrorCode::LoginErrorNone,
        };
    }

    auto LoginService::KickAsync(KickParam param) -> Future<KickResult>
    {
        co_await *_strand;
        assert(ExecutionContext::GetExecutor() == _strand.get());

        const auto iter = _loginAccounts.find(param.account);
        if (iter == _loginAccounts.end())
        {
            co_return KickResult{
                .errorCode = LoginServiceErrorCode::LoginErrorFailInvalid,
            };
        }

        AuthToken token = std::move(iter->second);
        _loginAccounts.erase(iter);

        [[maybe_unused]] const bool erased = _states.erase(token.values);
        assert(erased);

        const KickPlayerParam kickParam{
            .token = std::move(token),
            .displayReason = "duplicated login",
        };
        (void)_locator.Find<IWorldService>()->KickPlayerAsync(kickParam);

        co_return KickResult{
            .errorCode = LoginServiceErrorCode::LoginErrorNone,
        };
    }

    void LoginService::StartTokenExpireTimer(AuthTokenState& state, std::chrono::system_clock::time_point createTime, std::string token)
    {
        assert(ExecutionContext::GetExecutor() == _strand.get());

        const auto now =  std::chrono::system_clock::now();
        const auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(state.GetExpireTime() - now);

        Future<void> handle = Delay(delay).Then(*_strand, [self = shared_from_this(), token = std::move(token), createTime]() mutable
            {
                self->HandleTokenExpireTimer(std::move(token), createTime);
            });

        state.SetExpireTimerHandle(std::move(handle));
    }

    void LoginService::HandleTokenExpireTimer(std::string token, std::chrono::system_clock::time_point createTime)
    {
        assert(ExecutionContext::GetExecutor() == _strand.get());

        if (_shutdown.load())
        {
            return;
        }

        auto iter = _states.find(token);
        if (iter == _states.end())
        {
            return;
        }

        if (iter->second->GetCreateTime() != createTime)
        {
            return;
        }

        const auto now = std::chrono::system_clock::now();
        if (iter->second->IsExpired(now))
        {
            const std::string& account = iter->second->GetAccount();
            ZEROSUGAR_LOG_INFO(_locator, std::format("[sl_login_service] auth_token removed by timer. account: {}",
                account));

            _loginAccounts.erase(account);
            _states.erase(iter);

            return;
        }

        StartTokenExpireTimer(*iter->second, createTime, std::move(token));
    }

    auto LoginService::MakeAuthToken() -> std::string
    {
        // TODO: use SPRNG
        std::uniform_int_distribution<uint32_t> dist;
        const uint32_t l = dist(_mt);
        const uint32_t r = dist(_mt);
        const uint64_t value = (static_cast<uint64_t>(l) << 32) | static_cast<uint64_t>(r);

        return std::to_string(value);
    }

    auto LoginService::MakeExpireTimePoint() -> std::chrono::system_clock::time_point
    {
        // TODO: make time service
        return std::chrono::system_clock::now() + std::chrono::seconds(180);
    }

    auto LoginService::GetRepositoryService() -> IRepositoryService&
    {
        auto* service = _locator.Find<IRepositoryService>();
        assert(service);

        return *service;
    }
}
