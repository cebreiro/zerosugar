#pragma once
#include <Eigen/Dense>
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class IPacket;
    class IGameController;
    class GameInstance;
    class GameSpatialSector;
}

namespace zerosugar::xr::detail::game
{
    class GameSpatialSet;
}

namespace zerosugar::xr
{
    class GameViewController
    {
    public:
        GameViewController() = delete;
        explicit GameViewController(GameInstance& instance);

        void ProcessMovement(game_entity_id_type id, const Eigen::Vector3d& position);


    private:
        template <typename T>
        void Broadcast(const T& packet, const detail::game::GameSpatialSet& set, std::optional<game_entity_id_type> excluded = std::nullopt);

        template <typename T>
        void Broadcast(const T& packet, const detail::game::GameSpatialSet& set, GameEntityType type, std::optional<game_entity_id_type> excluded = std::nullopt);

        void Broadcast(const IPacket& packet, int32_t opcode, const detail::game::GameSpatialSet& set, std::optional<game_entity_id_type> excluded);
        void Broadcast(const IPacket& packet, int32_t opcode, const detail::game::GameSpatialSet& set, GameEntityType type, std::optional<game_entity_id_type> excluded);

        template <typename T>
        void Send(const T& packet, IGameController& controller);

        void Send(const IPacket& packet, int32_t opcode, IGameController& controller);

    private:
        GameInstance& _gameInstance;
    };
}
