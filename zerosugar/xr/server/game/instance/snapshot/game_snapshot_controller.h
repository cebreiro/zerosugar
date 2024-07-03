#pragma once
#include <Eigen/Dense>
#include "zerosugar/xr/data/enum/equip_position.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    struct InventoryItem;

    class IPacket;
    class IGameController;
    class GameEntity;
    class GameInstance;
    class GameSpatialSector;
    class GamePlayerSnapshot;
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
        void ProcessPlayerDespawn(game_entity_id_type entityId);
        void ProcessMovement(game_entity_id_type id, const Eigen::Vector3d& position);
        void ProcessStop(game_entity_id_type id, const Eigen::Vector3d& position);
        void ProcessSprint(game_entity_id_type id);
        void ProcessRollDodge(game_entity_id_type id, const Eigen::Vector3d& rotation);

        void ProcessPlayerEquipItemChange(game_entity_id_type id, data::EquipPosition pos, const InventoryItem* item);

    private:
        GameInstance& _gameInstance;
    };
}
