#pragma once
#include <cstdint>
#include <chrono>

namespace zerosugar::sl
{
    class AuthTokenStatus
    {
    public:
        AuthTokenStatus() = default;
        AuthTokenStatus(int64_t accountId, std::string account, std::chrono::system_clock::time_point expireTimePoint);

        bool IsExpired(std::chrono::system_clock::time_point timePoint) const;

        auto GetAccountId() const -> int64_t;
        auto GetAccount() const -> const std::string&;
        auto GetExpireTime() const -> const std::chrono::system_clock::time_point&;

        void SetExpireTimePoint(std::chrono::system_clock::time_point timePoint);

    private:
        int64_t _accountId = 0;
        std::string _account;
        std::chrono::system_clock::time_point _expireTimePoint = std::chrono::system_clock::time_point::min();
    };
}
