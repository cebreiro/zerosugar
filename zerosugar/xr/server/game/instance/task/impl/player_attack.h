#pragma once
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    class PlayerAttack final : public GameTaskBaseParamT<IPacket, network::game::cs::StartPlayerAttack, MainTargetSelector>
    {
    public:
        PlayerAttack(UniquePtrNotNull<network::game::cs::StartPlayerAttack> param, game_entity_id_type targetId,
            game_time_point_type creationTimePoint = game_clock_type::now());

    private:
        void Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        Eigen::Vector3d _resultPosition;
        Eigen::Vector3d _resultRotation;
    };
}
