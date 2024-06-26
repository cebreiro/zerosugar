#include "movement_component.h"

namespace zerosugar::xr
{
    auto MovementComponent::GetPosition() const -> const Eigen::Vector3d&
    {
        return _position;
    }

    auto MovementComponent::GetX() const -> double
    {
        return _position.x();
    }

    auto MovementComponent::GetY() const -> double
    {
        return _position.y();
    }

    auto MovementComponent::GetZ() const -> double
    {
        return _position.z();
    }

    auto MovementComponent::GetYaw() const -> double
    {
        return _yaw;
    }

    void MovementComponent::SetPosition(const Eigen::Vector3d& position)
    {
        _position = position;
    }
}
