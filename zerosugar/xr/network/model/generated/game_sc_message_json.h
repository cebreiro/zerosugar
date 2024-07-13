#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/network/model/generated/game_message.h"
#include "zerosugar/xr/network/model/generated/game_message_json.h"

namespace zerosugar::xr::network::game::sc
{
    void from_json(const nlohmann::json& j, EnterGame& item);
    void to_json(nlohmann::json& j, const EnterGame& item);

    void from_json(const nlohmann::json& j, NotifyPlayerActivated& item);
    void to_json(nlohmann::json& j, const NotifyPlayerActivated& item);

    void from_json(const nlohmann::json& j, SpawnRemotePlayer& item);
    void to_json(nlohmann::json& j, const SpawnRemotePlayer& item);

    void from_json(const nlohmann::json& j, AddRemotePlayer& item);
    void to_json(nlohmann::json& j, const AddRemotePlayer& item);

    void from_json(const nlohmann::json& j, RemoveRemotePlayer& item);
    void to_json(nlohmann::json& j, const RemoveRemotePlayer& item);

    void from_json(const nlohmann::json& j, RemotePlayerAttack& item);
    void to_json(nlohmann::json& j, const RemotePlayerAttack& item);

    void from_json(const nlohmann::json& j, BeAttackedPlayer& item);
    void to_json(nlohmann::json& j, const BeAttackedPlayer& item);

    void from_json(const nlohmann::json& j, SpawnMonster& item);
    void to_json(nlohmann::json& j, const SpawnMonster& item);

    void from_json(const nlohmann::json& j, AddMonster& item);
    void to_json(nlohmann::json& j, const AddMonster& item);

    void from_json(const nlohmann::json& j, RemoveMonster& item);
    void to_json(nlohmann::json& j, const RemoveMonster& item);

    void from_json(const nlohmann::json& j, DespawnMonster& item);
    void to_json(nlohmann::json& j, const DespawnMonster& item);

    void from_json(const nlohmann::json& j, MoveMonster& item);
    void to_json(nlohmann::json& j, const MoveMonster& item);

    void from_json(const nlohmann::json& j, AttackMonster& item);
    void to_json(nlohmann::json& j, const AttackMonster& item);

    void from_json(const nlohmann::json& j, BeAttackedMonster& item);
    void to_json(nlohmann::json& j, const BeAttackedMonster& item);

    void from_json(const nlohmann::json& j, MoveRemotePlayer& item);
    void to_json(nlohmann::json& j, const MoveRemotePlayer& item);

    void from_json(const nlohmann::json& j, StopRemotePlayer& item);
    void to_json(nlohmann::json& j, const StopRemotePlayer& item);

    void from_json(const nlohmann::json& j, SprintRemotePlayer& item);
    void to_json(nlohmann::json& j, const SprintRemotePlayer& item);

    void from_json(const nlohmann::json& j, RollDodgeRemotePlayer& item);
    void to_json(nlohmann::json& j, const RollDodgeRemotePlayer& item);

    void from_json(const nlohmann::json& j, NotifyChattingMessage& item);
    void to_json(nlohmann::json& j, const NotifyChattingMessage& item);

    void from_json(const nlohmann::json& j, AddInventory& item);
    void to_json(nlohmann::json& j, const AddInventory& item);

    void from_json(const nlohmann::json& j, RemoveInventory& item);
    void to_json(nlohmann::json& j, const RemoveInventory& item);

    void from_json(const nlohmann::json& j, NotifySwapItemResult& item);
    void to_json(nlohmann::json& j, const NotifySwapItemResult& item);

    void from_json(const nlohmann::json& j, ChangeRemotePlayerEquipItem& item);
    void to_json(nlohmann::json& j, const ChangeRemotePlayerEquipItem& item);

    void from_json(const nlohmann::json& j, NotifyDungeonMatchGroupCreation& item);
    void to_json(nlohmann::json& j, const NotifyDungeonMatchGroupCreation& item);

    void from_json(const nlohmann::json& j, NotifyDungeonMatchFailure& item);
    void to_json(nlohmann::json& j, const NotifyDungeonMatchFailure& item);

    void from_json(const nlohmann::json& j, NotifyDungeonMatchGroupApproved& item);
    void to_json(nlohmann::json& j, const NotifyDungeonMatchGroupApproved& item);

    void from_json(const nlohmann::json& j, SpawnerMonsterDead& item);
    void to_json(nlohmann::json& j, const SpawnerMonsterDead& item);

}
