#pragma once
#include <Eigen/Dense>
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class GameEntity;
}

namespace zerosugar::xr
{
    class GameEntityView
    {
    public:
        explicit GameEntityView(const GameEntity& entity);

        auto GetId() const -> game_entity_id_type;
        auto GetPosition() const -> const Eigen::Vector3d&;

        void SetPosition(const Eigen::Vector3d& position);

    private:
        game_entity_id_type _id;

        Eigen::Vector3d _position;
    };
}
