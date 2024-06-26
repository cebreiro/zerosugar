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

    void from_json(const nlohmann::json& j, PlayerBase& item);
    void to_json(nlohmann::json& j, const PlayerBase& item);

    void from_json(const nlohmann::json& j, Equipment& item);
    void to_json(nlohmann::json& j, const Equipment& item);

    void from_json(const nlohmann::json& j, PlayerEquipment& item);
    void to_json(nlohmann::json& j, const PlayerEquipment& item);

    void from_json(const nlohmann::json& j, PlayerInventoryItem& item);
    void to_json(nlohmann::json& j, const PlayerInventoryItem& item);

    void from_json(const nlohmann::json& j, RemotePlayer& item);
    void to_json(nlohmann::json& j, const RemotePlayer& item);

    void from_json(const nlohmann::json& j, Player& item);
    void to_json(nlohmann::json& j, const Player& item);

    void from_json(const nlohmann::json& j, Monster& item);
    void to_json(nlohmann::json& j, const Monster& item);

}
