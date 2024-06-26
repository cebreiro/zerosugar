#pragma once
#include <Eigen/Dense>

#include "zerosugar/xr/server/game/instance/component/game_component.h"

namespace zerosugar::xr
{
    class MovementComponent : public GameComponent
    {
    public:
        auto GetPosition() const -> const Eigen::Vector3d&;
        auto GetX() const -> double;
        auto GetY() const -> double;
        auto GetZ() const -> double;
        auto GetYaw() const -> double;

        void SetPosition(const Eigen::Vector3d& position);

    private:
        Eigen::Vector3d _position;
        double _yaw = 0.f;
    };
}
