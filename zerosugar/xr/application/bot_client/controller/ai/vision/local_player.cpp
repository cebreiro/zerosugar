#include "local_player.h"

namespace zerosugar::xr::bot
{
    LocalPlayer::LocalPlayer(const network::game::Player& data, int32_t zoneId)
        : _data(data)
        , _zoneId(zoneId)
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

    bool LocalPlayer::IsAttackState(int32_t attackId, std::chrono::system_clock::time_point timePoint) const
    {
        return _attackId == attackId && timePoint < _attackEndTimePoint;
    }

    void LocalPlayer::SetAttack(int32_t attackId, std::chrono::system_clock::time_point endTimePoint)
    {
        _attackId = attackId;
        _attackEndTimePoint = endTimePoint;
    }

    auto LocalPlayer::GetData() const -> const network::game::Player&
    {
        return _data;
    }

    auto LocalPlayer::GetZoneId() const -> int32_t
    {
        return _zoneId;
    }

    auto LocalPlayer::GetPosition() const -> const Eigen::Vector3d&
    {
        return _position;
    }

    auto LocalPlayer::GetRotation() const -> const Eigen::Vector3d&
    {
        return _rotation;
    }

    void LocalPlayer::SetPosition(const Eigen::Vector3d& position)
    {
        _position = position;
    }

    void LocalPlayer::SetRotation(const Eigen::Vector3d& rotation)
    {
        _rotation = rotation;
    }
}
