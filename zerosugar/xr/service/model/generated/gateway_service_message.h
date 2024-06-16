#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

namespace zerosugar::xr::service
{
    enum class GatewayServiceErrorCode : int64_t
    {
        GatewayErrorNone = 0,
        AddGameErrorNameDuplicated = 10001,

    };
    auto GetEnumName(GatewayServiceErrorCode e) -> std::string_view;

    struct GameServiceEndpoint
    {
        std::string name = {};
        std::string ip = {};
        int32_t port = {};
    };

    struct AddGameServiceParam
    {
        GameServiceEndpoint address = {};
    };

    struct AddGameServiceResult
    {
        GatewayServiceErrorCode errorCode = {};
    };

    struct GetGameServiceListParam
    {
    };

    struct GetGameServiceListResult
    {
        GatewayServiceErrorCode errorCode = {};
        std::vector<GameServiceEndpoint> addresses = {};
    };

}
