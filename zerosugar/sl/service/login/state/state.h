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
        AuthTokenState(int64_t accountId, std::string account, std::string address,
            std::chrono::system_clock::time_point expireTimePoint);
        ~AuthTokenState();

        bool IsExpired(std::chrono::system_clock::time_point timePoint) const;

        auto GetAccountId() const -> int64_t;
        auto GetAccount() const -> const std::string&;
        auto GetAddress() const -> const std::string&;
        auto GetCreateTime() const-> std::chrono::system_clock::time_point;
        auto GetExpireTime() const -> std::chrono::system_clock::time_point;

        void SetExpireTimePoint(std::chrono::system_clock::time_point timePoint);
        void SetExpireTimerHandle(Future<void> handle);

    private:
        int64_t _accountId = 0;
        std::string _account;
        std::string _address;
        std::chrono::system_clock::time_point _createTimePoint = std::chrono::system_clock::time_point::min();
        std::chrono::system_clock::time_point _expireTimePoint = std::chrono::system_clock::time_point::min();
        Future<void> _expireTimerHandle;
    };
}
