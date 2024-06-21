#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/network/model/generated/game_message.h"

namespace zerosugar::xr::network::game
{
    void from_json(const nlohmann::json& j, Position& item);
    void to_json(nlohmann::json& j, const Position& item);

    void from_json(const nlohmann::json& j, Rotation& item);
    void to_json(nlohmann::json& j, const Rotation& item);

    void from_json(const nlohmann::json& j, Transform& item);
    void to_json(nlohmann::json& j, const Transform& item);

    void from_json(const nlohmann::json& j, CharacterStat& item);
    void to_json(nlohmann::json& j, const CharacterStat& item);

    void from_json(const nlohmann::json& j, Equipment& item);
    void to_json(nlohmann::json& j, const Equipment& item);

    void from_json(const nlohmann::json& j, CharacterEquipment& item);
    void to_json(nlohmann::json& j, const CharacterEquipment& item);

    void from_json(const nlohmann::json& j, CharacterInventoryItem& item);
    void to_json(nlohmann::json& j, const CharacterInventoryItem& item);

    void from_json(const nlohmann::json& j, Character& item);
    void to_json(nlohmann::json& j, const Character& item);

    void from_json(const nlohmann::json& j, Monster& item);
    void to_json(nlohmann::json& j, const Monster& item);

}
