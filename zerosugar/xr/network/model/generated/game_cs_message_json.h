#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/network/model/generated/game_message.h"
#include "zerosugar/xr/network/model/generated/game_message_json.h"

namespace zerosugar::xr::network::game::cs
{
    void from_json(const nlohmann::json& j, Authenticate& item);
    void to_json(nlohmann::json& j, const Authenticate& item);

    void from_json(const nlohmann::json& j, MovePlayer& item);
    void to_json(nlohmann::json& j, const MovePlayer& item);

    void from_json(const nlohmann::json& j, StopPlayerMovement& item);
    void to_json(nlohmann::json& j, const StopPlayerMovement& item);

}
