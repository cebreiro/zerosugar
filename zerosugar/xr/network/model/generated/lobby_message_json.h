#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/network/model/generated/lobby_message.h"

namespace zerosugar::xr::network::lobby
{
    void from_json(const nlohmann::json& j, LobbyCharacter& item);
    void to_json(nlohmann::json& j, const LobbyCharacter& item);

}
