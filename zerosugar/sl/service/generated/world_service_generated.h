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
    enum class WorldServiceErrorCode : int64_t
    {
        WorldErrorNone = 0,
        WorldErrorWorldNotFound = 1,
        WorldErrorWorldAlreadyExists = 1000,
        WorldErrorZoneAlreadyExists = 10001,
    };
    struct World
    {
        int32_t worldId = {};
        std::string address = {};
    };

    struct CreateWorldParam
    {
        int32_t worldId = {};
        std::string address = {};
    };

    struct CreateWorldResult
    {
        WorldServiceErrorCode errorCode = {};
    };

    struct CreateZoneParam
    {
        int32_t worldId = {};
        int32_t zoneId = {};
        std::string address = {};
        int32_t port = {};
    };

    struct CreateZoneResult
    {
        WorldServiceErrorCode errorCode = {};
    };

    struct GetWorldListParam
    {
    };

    struct GetWorldListResult
    {
        WorldServiceErrorCode errorCode = {};
        std::vector<World> worlds = {};
    };

    struct EnterWorldParam
    {
        int32_t worldId = {};
        AuthToken token = {};
        int64_t accountId = {};
        std::string account = {};
    };

    struct EnterWorldResult
    {
        WorldServiceErrorCode errorCode = {};
    };

    struct KickPlayerParam
    {
        AuthToken token = {};
        std::optional<std::string> displayReason = {};
    };

    struct KickPlayerResult
    {
        WorldServiceErrorCode errorCode = {};
    };

}
