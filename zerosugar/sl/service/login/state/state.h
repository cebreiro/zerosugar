#pragma once
#include <cstdint>
#include <chrono>

namespace zerosugar::sl
{
    class AuthTokenState
    {
    public:
        AuthTokenState() = default;
        AuthTokenState(const AuthTokenState& other) = delete;
        AuthTokenState(AuthTokenState&& other) noexcept = default;
        AuthTokenState& operator=(const AuthTokenState& other) = delete;
        AuthTokenState& operator=(AuthTokenState&& other) noexcept = default;

    public:
        AuthTokenState(int64_t accountId, std::string account, std::chrono::system_clock::time_point expireTimePoint);
        ~AuthTokenState();

        bool IsExpired(std::chrono::system_clock::time_point timePoint) const;

        auto GetAccountId() const -> int64_t;
        auto GetAccount() const -> const std::string&;
        auto GetExpireTime() const -> const std::chrono::system_clock::time_point&;

        void SetExpireTimePoint(std::chrono::system_clock::time_point timePoint);
        void SetExpireTimerHandle(Future<void> handle);

    private:
        int64_t _accountId = 0;
        std::string _account;
        std::chrono::system_clock::time_point _expireTimePoint = std::chrono::system_clock::time_point::min();
        Future<void> _expireTimerHandle;
    };
}
