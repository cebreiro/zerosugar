#include "state.h"

namespace zerosugar::sl
{
    AuthTokenStatus::AuthTokenStatus(int64_t accountId, std::string account,
        std::chrono::system_clock::time_point expireTimePoint)
        : _accountId(accountId)
        , _account(std::move(account))
        , _expireTimePoint(expireTimePoint)
    {
    }

    bool AuthTokenStatus::IsExpired(std::chrono::system_clock::time_point timePoint) const
    {
        return timePoint >= _expireTimePoint;
    }

    auto AuthTokenStatus::GetAccountId() const -> int64_t
    {
        return _accountId;
    }

    void AuthTokenStatus::SetExpireTimePoint(std::chrono::system_clock::time_point timePoint)
    {
        _expireTimePoint = timePoint;
    }

    auto AuthTokenStatus::GetAccount() const -> const std::string&
    {
        return _account;
    }

    auto AuthTokenStatus::GetExpireTime() const -> const std::chrono::system_clock::time_point&
    {
        return _expireTimePoint;
    }
}
