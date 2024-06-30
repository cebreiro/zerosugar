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

    void from_json(const nlohmann::json& j, StopPlayer& item);
    void to_json(nlohmann::json& j, const StopPlayer& item);

    void from_json(const nlohmann::json& j, SprintPlayer& item);
    void to_json(nlohmann::json& j, const SprintPlayer& item);

    void from_json(const nlohmann::json& j, RollDodgePlayer& item);
    void to_json(nlohmann::json& j, const RollDodgePlayer& item);

    void from_json(const nlohmann::json& j, Chat& item);
    void to_json(nlohmann::json& j, const Chat& item);

    void from_json(const nlohmann::json& j, SwapItem& item);
    void to_json(nlohmann::json& j, const SwapItem& item);

}
