#include "state.h"

namespace zerosugar::sl
{
    AuthTokenState::AuthTokenState(int64_t accountId, std::string account,
        std::chrono::system_clock::time_point expireTimePoint)
        : _accountId(accountId)
        , _account(std::move(account))
        , _expireTimePoint(expireTimePoint)
    {
    }

    AuthTokenState::~AuthTokenState()
    {
        if (_expireTimerHandle.IsValid())
        {
            (void)_expireTimerHandle.Cancel();
        }
    }

    bool AuthTokenState::IsExpired(std::chrono::system_clock::time_point timePoint) const
    {
        return timePoint >= _expireTimePoint;
    }

    auto AuthTokenState::GetAccountId() const -> int64_t
    {
        return _accountId;
    }

    void AuthTokenState::SetExpireTimePoint(std::chrono::system_clock::time_point timePoint)
    {
        _expireTimePoint = timePoint;
    }

    void AuthTokenState::SetExpireTimerHandle(Future<void> handle)
    {
        _expireTimerHandle = std::move(handle);
    }

    auto AuthTokenState::GetAccount() const -> const std::string&
    {
        return _account;
    }

    auto AuthTokenState::GetExpireTime() const -> const std::chrono::system_clock::time_point&
    {
        return _expireTimePoint;
    }
}
