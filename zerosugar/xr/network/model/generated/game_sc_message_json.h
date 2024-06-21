#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/network/model/generated/game_sc_message.h"

namespace zerosugar::xr::network::game::sc
{
    void from_json(const nlohmann::json& j, Test& item);
    void to_json(nlohmann::json& j, const Test& item);

}
