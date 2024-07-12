#pragma once
#include "zerosugar/xr/network/model/generated/game_message.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/visual_object_interface.h"

namespace zerosugar::xr::bot
{
    class LocalPlayer final : public IVisualObject
    {
    public:
        LocalPlayer() = delete;
        LocalPlayer(const network::game::Player& data, int32_t zoneId);

        bool IsAttackState(int32_t attackId, std::chrono::system_clock::time_point timePoint) const;

        void SetAttack(int32_t attackId, std::chrono::system_clock::time_point endTimePoint);

        auto GetData() const -> const network::game::Player&;
        auto GetZoneId() const -> int32_t;
        auto GetPosition() const -> const Eigen::Vector3d & override;
        auto GetRotation() const -> const Eigen::Vector3d & override;

        void SetPosition(const Eigen::Vector3d& position);
        void SetRotation(const Eigen::Vector3d& rotation);

    private:
        network::game::Player _data;
        int32_t _zoneId;
        Eigen::Vector3d _position;
        Eigen::Vector3d _rotation;

        int32_t _attackId = 0;
        std::chrono::system_clock::time_point _attackEndTimePoint;
    };
}
