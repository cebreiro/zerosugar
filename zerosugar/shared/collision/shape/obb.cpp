#include "obb.h"

namespace zerosugar::collision
{
    OBB::OBB(Eigen::Vector2d center, Eigen::Vector2d halfSize, Eigen::Matrix2d rotation)
        : _center(std::move(center))
        , _halfSize(std::move(halfSize))
        , _rotation(std::move(rotation))
    {
    }

    auto OBB::GetCenter() const -> Eigen::Vector2d
    {
        return _center;
    }

    auto OBB::GetHalfSize() const -> Eigen::Vector2d
    {
        return _halfSize;
    }

    auto OBB::GetRotation() const -> Eigen::Matrix2d
    {
        return _rotation;
    }

    OBB3d::OBB3d(const Eigen::Vector3d& center, const Eigen::Vector3d& halfSize, const Eigen::Matrix3d& rotation)
        : _center(center)
        , _halfSize(halfSize)
        , _rotation(rotation)
    {
    }

    auto OBB3d::GetCenter() const -> const Eigen::Vector3d&
    {
        return _center;
    }

    auto OBB3d::GetHalfSize() const -> const Eigen::Vector3d&
    {
        return _halfSize;
    }

    auto OBB3d::GetRotation() const -> const Eigen::Matrix3d&
    {
        return _rotation;
    }
}
