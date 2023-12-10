#include "login_service.h"

#include "zerosugar/shared/service/service_locator_log.h"
#include "zerosugar/shared/execution/future/future_coroutine_traits.h"
#include "zerosugar/shared/execution/executor/strand.h"
#include "zerosugar/shared/execution/executor/executor_coroutine_traits.h"
#include "zerosugar/shared/execution/future/operation/delay.h"

namespace zerosugar::sl
{
    using namespace service;

    LoginService::LoginService(Locator locator, execution::IExecutor& executor)
        : _locator(std::move(locator))
        , _strand(std::make_shared<Strand>(executor.SharedFromThis()))
        , _mt(std::random_device()())
    {
        assert(_locator.Find<service::IRepositoryService>());
    }

    void LoginService::Shutdown()
    {
        _shutdown.store(true);
    }

    auto LoginService::LoginAsync(LoginParam param) -> Future<LoginResult>
    {
        co_await *_strand;
        assert(ExecutionContext::GetExecutor() == _strand.get());

        if (_loginAccounts.contains(param.account))
        {
            ZEROSUGAR_LOG_INFO(_locator, std::format("[sl_login_service] login fail. account is not found. context: {}, account: {}",
                param.context, param.account));

            co_return LoginResult{
                .errorCode = LoginServiceErrorCode::LoginErrorFailInvalid,
            };
        }

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

        if (!_loginAccounts.insert(param.account).second)
        {
            co_return LoginResult{
                .errorCode = LoginServiceErrorCode::LoginErrorFailInvalid,
            };
        }

        AuthToken token;
        AuthTokenStatus status(account->id, account->account, MakeExpireTimePoint());
        do
        {
            token = MakeAuthToken();

            const auto& [iter, inserted] = _statuses.try_emplace(std::move(MakeAuthToken().values), status);
            if (inserted)
            {
                token.values = iter->first;
                break;
            }
        } while (true);

        ZEROSUGAR_LOG_INFO(_locator, std::format("[sl_login_service] token generated. account: {}, token: {}",
            param.account, token.values));

        StartTokenExpireTimer(token, status);

        co_return LoginResult{
            .errorCode = LoginServiceErrorCode::LoginErrorNone,
            .token = std::move(token),
            .accountId = account->id,
        };
    }

    auto LoginService::LogoutAsync(LogoutParam param) -> Future<LogoutResult>
    {
        co_await *_strand;
        assert(ExecutionContext::GetExecutor() == _strand.get());

        auto iter = _statuses.find(param.token.values);
        if (iter == _statuses.end())
        {
            // TODO: add virtual auto IService::GetName() const = 0 and use it
            ZEROSUGAR_LOG_INFO(_locator, std::format("[sl_login_service] login fail. auth_token is not found. context: {}",
                param.context));

            co_return LogoutResult{
                .errorCode = LoginServiceErrorCode::LoginErrorFailInvalid,
            };
        }

        const std::string& account = iter->second.GetAccount();
        ZEROSUGAR_LOG_INFO(_locator, std::format("[sl_login_service] logout. account: {}, context: {}",
            account, param.context));

        _loginAccounts.erase(account);
        _statuses.erase(iter);

        co_return LogoutResult{
             .errorCode = LoginServiceErrorCode::LoginErrorNone,
        };
    }

    void LoginService::StartTokenExpireTimer(const AuthToken& token, const AuthTokenStatus& status)
    {
        assert(ExecutionContext::GetExecutor() == _strand.get());

        const auto now =  std::chrono::system_clock::now();
        const auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(status.GetExpireTime() - now);

        Delay(delay).Then(*_strand, [self = shared_from_this(), token = token]()
            {
                self->HandleTokenExpireTimer(token);
            });
    }

    void LoginService::HandleTokenExpireTimer(const AuthToken& token)
    {
        assert(ExecutionContext::GetExecutor() == _strand.get());

        if (_shutdown.load())
        {
            return;
        }

        auto iter = _statuses.find(token.values);
        if (iter == _statuses.end())
        {
            return;
        }

        const AuthTokenStatus& status = iter->second;

        const auto now = std::chrono::system_clock::now();
        if (status.IsExpired(now))
        {
            const std::string& account = iter->second.GetAccount();
            ZEROSUGAR_LOG_INFO(_locator, std::format("[sl_login_service] auth_token removed by timer. account: {}",
                account));

            _loginAccounts.erase(account);
            _statuses.erase(iter);

            return;
        }

        StartTokenExpireTimer(token, iter->second);
    }

    auto LoginService::MakeAuthToken() -> AuthToken
    {
        // TODO: use SPRNG
        std::uniform_int_distribution<int32_t> dist;
        const int32_t l = dist(_mt);
        const int32_t r = dist(_mt);
        const int64_t value = (static_cast<int64_t>(l) << 32) | static_cast<int64_t>(r);

        return AuthToken{
            .values = std::to_string(value),
        };
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
