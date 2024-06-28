#pragma once
#include <Eigen/Dense>
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    class PlayerMove final : public GameTaskBaseParamT<IPacket, network::game::cs::MovePlayer, MainTargetSelector>
    {
    public:
        PlayerMove(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId,
            std::chrono::system_clock::time_point creationTimePoint = std::chrono::system_clock::now());

    private:
        void Execute(GameExecutionParallel& parallel, MainTargetSelector::target_type target) override;
        void OnComplete(GameExecutionSerial& serial) override;

    private:
        game_entity_id_type _id;
        Eigen::Vector3d _newPosition;
    };
}
