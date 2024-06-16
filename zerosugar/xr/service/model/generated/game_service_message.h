#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

namespace zerosugar::xr::service
{
    enum class GameServiceErrorCode : int64_t
    {
        GameErrorNone = 0,
        GameErrorInternalError = 1,
        RequestSnowflakeKeyErrorOutOfPool = 30001,
        ReturnSnowflakeKeyErrorInvalidKey = 40001,

    };
    auto GetEnumName(GameServiceErrorCode e) -> std::string_view;

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
        GameServiceErrorCode errorCode = {};
        int32_t snowflakeKey = {};
    };

    struct ReturnSnowflakeKeyParam
    {
        std::string requester = {};
        int32_t snowflakeKey = {};
    };

    struct ReturnSnowflakeKeyResult
    {
        GameServiceErrorCode errorCode = {};
    };

}
