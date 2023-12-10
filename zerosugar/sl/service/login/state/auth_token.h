#pragma once
#include <cstdint>
#include <array>
#include <string>
#include <optional>
#include <format>

namespace zerosugar::sl
{
    class AuthToken
    {
    public:
        AuthToken() = default;
        AuthToken(int32_t key1, int32_t key2);

        auto ToString() const -> std::string;
        auto ToArray() const -> std::array<int32_t, 2>;

        static auto FromString(std::string_view str) -> std::optional<AuthToken>;

        friend bool operator==(const AuthToken& lhs, const AuthToken& rhs);
        friend bool operator!=(const AuthToken& lhs, const AuthToken& rhs);
        friend bool operator<(const AuthToken& lhs, const AuthToken& rhs);

    private:
        int32_t _key1 = 0;
        int32_t _key2 = 0;
    };
}

namespace std
{
    template <>
    struct hash<zerosugar::sl::AuthToken>
    {
        auto operator()(const zerosugar::sl::AuthToken& token) const noexcept -> size_t
        {
            const auto& valueArray = token.ToArray();
            const int64_t value = (static_cast<int64_t>(valueArray[0]) << 32) | valueArray[1];

            return std::hash<int64_t>()(value);
        }
    };

    template <>
    struct formatter<zerosugar::sl::AuthToken> : formatter<string>
    {
        auto format(zerosugar::sl::AuthToken token, format_context& ctx) const
        {
            return formatter<string>::format(
                std::format("{}", token.ToString()), ctx);
        }
    };
}
