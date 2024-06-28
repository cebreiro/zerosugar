#pragma once
#include <Eigen/Dense>
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    class PlayerMove : public GameTaskT<IPacket, network::game::cs::MovePlayer, MainTargetSelector>
    {
    public:
        PlayerMove(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId);

    private:
        void Execute(GameExecutionParallel& parallel, MainTargetSelector::target_type target) override;
        void OnComplete(GameExecutionSerial& serial) override;

    private:
        game_entity_id_type _id;
        Eigen::Vector3d _newPosition;
    };
}
