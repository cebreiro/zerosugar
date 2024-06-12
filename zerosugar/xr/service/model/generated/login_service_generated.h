#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

namespace zerosugar::xr::service
{
    enum class LoginServiceErrorCode : int64_t
    {
        LoginErrorNone = 0,
        LoginErrorFailInvalid = 1,

    };
    auto GetEnumName(LoginServiceErrorCode e) -> std::string_view;

    struct LoginParam
    {
        std::string account = {};
        std::string password = {};
    };

    struct LoginResult
    {
        LoginServiceErrorCode errorCode = {};
    };

    struct CreateAccountParam
    {
        std::string account = {};
        std::string password = {};
    };

    struct CreateAccountResult
    {
        LoginServiceErrorCode errorCode = {};
    };

}
