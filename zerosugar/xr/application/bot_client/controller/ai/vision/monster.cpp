#include "monster.h"

namespace zerosugar::xr::bot
{
    Monster::Monster(const network::game::Monster& data)
        : _data(data)
        , _position([&]() -> Eigen::Vector3d
            {
                const auto& pos = _data.transform.position;

                return Eigen::Vector3d(pos.x, pos.y, pos.z);
            }())
        , _rotation([&]() -> Eigen::Vector3d
            {
                const auto& rot = _data.transform.rotation;

                return Eigen::Vector3d(rot.pitch, rot.yaw, rot.roll);
            }())
    {
    }

    auto Monster::GetData() const -> const network::game::Monster&
    {
        return _data;
    }

    auto Monster::GetPosition() const -> const Eigen::Vector3d&
    {
        return _position;
    }

    auto Monster::GetRotation() const -> const Eigen::Vector3d&
    {
        return _rotation;
    }

    void Monster::SetPosition(const Eigen::Vector3d& position)
    {
        _position = position;
    }

    void Monster::SetRotation(const Eigen::Vector3d& rotation)
    {
        _rotation = rotation;
    }
}
