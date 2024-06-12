#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/sl/service/generated/shared_generated.h"

namespace zerosugar::sl::service
{
    enum class LoginServiceErrorCode : int64_t
    {
        LoginErrorNone = 0,
        LoginErrorFailInvalid = 1,
        LoginErrorFailLoginDuplicate = 10000,
        LoginErrorFailAuthenticateFailure = 20000,
        LoginErrorFailAuthorizeAccessDenied = 30000,
    };
    enum class AuthorizedAccessType : int64_t
    {
        AuthorizedAccessNone = 0,
        AuthorizedAccessCharacterCreate = 1,
        AuthorizedAccessCharacterDelete = 2,
        AuthorizedAccessCharacterEnterZone = 3,
    };
    struct LoginParam
    {
        std::string account = {};
        std::string password = {};
        std::string address = {};
        std::string context = {};
    };

    struct LoginResult
    {
        LoginServiceErrorCode errorCode = {};
        AuthToken token = {};
        int64_t accountId = {};
    };

    struct LogoutParam
    {
        AuthToken token = {};
        std::string context = {};
    };

    struct LogoutResult
    {
        LoginServiceErrorCode errorCode = {};
    };

    struct AuthenticateParam
    {
        AuthToken token = {};
        std::string address = {};
    };

    struct AuthenticateResult
    {
        LoginServiceErrorCode errorCode = {};
        int64_t accountId = {};
    };

    struct AuthorizeParam
    {
        AuthToken token = {};
        AuthorizedAccessType type = {};
        int32_t typeValue1 = {};
        int32_t typeValue2 = {};
    };

    struct AuthorizeResult
    {
        LoginServiceErrorCode errorCode = {};
    };

    struct KickParam
    {
        std::string account = {};
        std::string context = {};
    };

    struct KickResult
    {
        LoginServiceErrorCode errorCode = {};
    };

}
