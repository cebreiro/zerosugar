#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

namespace zerosugar::xr::service
{
    enum class CoordinationServiceErrorCode : int64_t
    {
        CoordinationErrorNone = 0,
        CoordinationErrorInternalError = 1,
        RequestSnowflakeKeyErrorOutOfPool = 30001,
        ReturnSnowflakeKeyErrorInvalidKey = 40001,

    };
    auto GetEnumName(CoordinationServiceErrorCode e) -> std::string_view;

    struct GetNameParam
    {
    };

    struct GetNameResult
    {
        std::string name = {};
    };

    struct RequestSnowflakeKeyParam
    {
        std::string requester = {};
    };

    struct RequestSnowflakeKeyResult
    {
        CoordinationServiceErrorCode errorCode = {};
        int32_t snowflakeKey = {};
    };

    struct ReturnSnowflakeKeyParam
    {
        std::string requester = {};
        int32_t snowflakeKey = {};
    };

    struct ReturnSnowflakeKeyResult
    {
        CoordinationServiceErrorCode errorCode = {};
    };

    struct AddPlayerParam
    {
        std::string authenticationToken = {};
        int64_t accountId = {};
        int64_t characterId = {};
        int32_t zoneId = {};
    };

    struct AddPlayerResult
    {
        CoordinationServiceErrorCode errorCode = {};
        std::string ip = {};
        int32_t port = {};
    };

}
