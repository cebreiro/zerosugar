#include "state.h"

namespace zerosugar::sl
{
    AuthTokenState::AuthTokenState(int64_t accountId, std::string account, std::string address,
        std::chrono::system_clock::time_point expireTimePoint)
        : _accountId(accountId)
        , _account(std::move(account))
        , _address(std::move(address))
        , _createTimePoint(std::chrono::system_clock::now())
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

    auto AuthTokenState::GetAddress() const -> const std::string&
    {
        return _address;
    }

    auto AuthTokenState::GetCreateTime() const -> std::chrono::system_clock::time_point
    {
        return _createTimePoint;
    }

    auto AuthTokenState::GetExpireTime() const -> std::chrono::system_clock::time_point
    {
        return _expireTimePoint;
    }
}
