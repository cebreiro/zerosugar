#pragma once
#include "zerosugar/xr/data/enum/equip_position.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"

namespace zerosugar::xr
{
    struct InventoryItem;

    class IPacket;
    class IGameController;
    class GameEntity;
    class GameInstance;
    class GameSpatialSector;
    class GamePlayerSnapshot;
    class GameMonsterSnapshot;
    class GameSpawnerSnapshot;
}

namespace zerosugar::xr::detail::game
{
    class GameSpatialSet;
}

namespace zerosugar::xr
{
    class GameSnapshotController
    {
    public:
        GameSnapshotController() = delete;

        explicit GameSnapshotController(GameInstance& instance);
        ~GameSnapshotController();

        void ProcessPlayerSpawn(const GameEntity& entity);
        void ProcessPlayerActivate(game_entity_id_type playerId);

        void ProcessPlayerDespawn(game_entity_id_type entityId);
        void ProcessMovement(game_entity_id_type playerId, const Eigen::Vector3d& position);
        void ProcessStop(game_entity_id_type playerId, const Eigen::Vector3d& position);
        void ProcessSprint(game_entity_id_type id);
        void ProcessRollDodge(game_entity_id_type id, const Eigen::Vector3d& rotation);

        void ProcessPlayerEquipItemChange(game_entity_id_type id, data::EquipPosition pos, const InventoryItem* item);

        void ProcessMonsterSpawn(const GameMonsterSnapshot& snapshot);
        void ProcessMonsterMove(GameMonsterSnapshot& snapshot, const Eigen::Vector3d& position, float yaw);
        void ProcessMonsterAttack(GameMonsterSnapshot& snapshot, int32_t attackIndex,
            const std::optional<Eigen::Vector3d>& destPos, const Eigen::Vector3d& rotation, double movementDuration);
        void ProcessMonsterAttackEffect(game_entity_id_type attackerId, int32_t attackId, game_entity_id_type targetId, float targetHP);

        void ProcessSpawnerAdd(const GameSpawnerSnapshot& snapshot);

    private:
        void HandlePlayerPositionChange(GamePlayerSnapshot& player, const Eigen::Vector3d& oldPos, const Eigen::Vector3d& newPos);
        void HandleMonsterPositionChange(GameMonsterSnapshot& monster, const Eigen::Vector3d& oldPos, const Eigen::Vector3d& newPos,
            bool syncMovement);

    private:
        GameInstance& _gameInstance;
    };
}
