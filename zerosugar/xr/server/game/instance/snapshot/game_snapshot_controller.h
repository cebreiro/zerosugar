#pragma once
#include <Eigen/Dense>
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
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
        void ProcessMovement(game_entity_id_type id, const Eigen::Vector3d& position);
        void ProcessStop(game_entity_id_type id, const Eigen::Vector3d& position);
        void ProcessSprint(game_entity_id_type id);
        void ProcessRollDodge(game_entity_id_type id, const Eigen::Vector3d& rotation);

        void Broadcast(const IPacket& packet, std::optional<game_entity_id_type> excluded = std::nullopt);
        void Broadcast(const IPacket& packet, const GamePlayerSnapshot& middle, std::optional<game_entity_id_type> excluded = std::nullopt);
        void Broadcast(const IPacket& packet, const detail::game::GameSpatialSet& set, std::optional<game_entity_id_type> excluded = std::nullopt);
        void Broadcast(const IPacket& packet, const detail::game::GameSpatialSet& set, GameEntityType type, std::optional<game_entity_id_type> excluded = std::nullopt);

        void Send(const IPacket& packet, IGameController& controller);

    private:
        GameInstance& _gameInstance;
    };
}
