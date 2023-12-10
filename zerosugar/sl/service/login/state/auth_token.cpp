#include "auth_token.h"

#include <format>
#include <vector>
#include <charconv>
#include <boost/algorithm/string.hpp>

namespace zerosugar::sl
{
    AuthToken::AuthToken(int32_t key1, int32_t key2)
        : _key1(key1)
        , _key2(key2)
    {
    }

    auto AuthToken::ToString() const -> std::string
    {
        return std::format("{{ {}:{} }}", static_cast<uint32_t>(_key1), static_cast<uint32_t>(_key2));
    }

    auto AuthToken::ToArray() const -> std::array<int32_t, 2>
    {
        return { _key1, _key2 };
    }

    auto AuthToken::FromString(std::string_view str) -> std::optional<AuthToken>
    {
        std::vector<std::string> result;
        boost::algorithm::split(result, str, boost::is_any_of(":"));

        if (result.size() < 2)
        {
            return {};
        }

        const std::string& key1 = result[0];
        const std::string& key2 = result[1];

        int32_t v1 = 0;
        if (std::from_chars(key1.c_str(), key1.c_str() + key1.size(), v1).ec == std::errc{})
        {
            return std::nullopt;
        }

        int32_t v2 = 0;
        if (std::from_chars(key2.c_str(), key2.c_str() + key2.size(), v2).ec == std::errc{})
        {
            return std::nullopt;
        }

        return AuthToken(v1, v2);
    }

    bool operator==(const AuthToken& lhs, const AuthToken& rhs)
    {
        if (lhs._key1 != rhs._key2)
        {
            return false;
        }

        if (lhs._key2 != rhs._key2)
        {
            return false;
        }

        return true;
    }

    bool operator!=(const AuthToken& lhs, const AuthToken& rhs)
    {
        return !(lhs == rhs);
    }

    bool operator<(const AuthToken& lhs, const AuthToken& rhs)
    {
        if (lhs._key1 < rhs._key2)
        {
            return true;
        }

        if (lhs._key2 < rhs._key2)
        {
            return true;
        }

        return false;
    }
}
