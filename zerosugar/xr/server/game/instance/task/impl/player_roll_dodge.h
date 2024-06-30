#pragma once
#include <Eigen/Dense>
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    class PlayerRollDodge final : public GameTaskBaseParamT<IPacket, network::game::cs::RollDodgePlayer, MainTargetSelector>
    {
    public:
        PlayerRollDodge(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId,
            game_time_point_type creationTimePoint = game_clock_type::now());

    private:
        void Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        game_entity_id_type _id;
        Eigen::Vector3d _rotation;
    };
}
