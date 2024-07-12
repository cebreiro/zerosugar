#pragma once
#include "zerosugar/xr/network/model/generated/game_message.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/visual_object_interface.h"

namespace zerosugar::xr::bot
{
    class Monster final
        : public IVisualObject
        , public std::enable_shared_from_this<Monster>
    {
    public:
        Monster() = delete;

        explicit Monster(const network::game::Monster& data);

        auto GetData() const -> const network::game::Monster&;
        auto GetPosition() const -> const Eigen::Vector3d& override;
        auto GetRotation() const -> const Eigen::Vector3d& override;

        void SetPosition(const Eigen::Vector3d& position);
        void SetRotation(const Eigen::Vector3d& rotation);

    private:
        network::game::Monster _data = {};
        Eigen::Vector3d _position;
        Eigen::Vector3d _rotation;
    };
}
