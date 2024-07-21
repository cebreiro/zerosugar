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
        LoginErrorInternalError = 2,
        AuthenticateErrorFail = 30001,
        RemoveAuthErrorTokenNotFound = 40001,

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
        std::string authenticationToken = {};
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

    struct AuthenticateParam
    {
        std::string token = {};
    };

    struct AuthenticateResult
    {
        LoginServiceErrorCode errorCode = {};
        int64_t accountId = {};
    };

    struct RemoveAuthParam
    {
        std::string token = {};
    };

    struct RemoveAuthResult
    {
        LoginServiceErrorCode errorCode = {};
    };

}
