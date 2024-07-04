#pragma once
#include <Eigen/Dense>

#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class IGameController;
}

namespace zerosugar::xr
{
    class GameSpawnerSnapshot
    {
    public:
        GameSpawnerSnapshot(IGameController& controller, game_entity_id_type entityId);
        ~GameSpawnerSnapshot();

        void Initialize(const GameEntity& entity);

        auto GetController() -> IGameController&;
        auto GetId() const -> game_entity_id_type;
        auto GetPosition() const -> const Eigen::Vector3d&;

    private:
        IGameController& _controller;
        game_entity_id_type _id;
        Eigen::Vector3d _position;
    };
}
