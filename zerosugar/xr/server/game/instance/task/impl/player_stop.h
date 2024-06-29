#pragma once
#include <Eigen/Dense>
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    class PlayerStop final : public GameTaskBaseParamT<IPacket, network::game::cs::StopPlayer, MainTargetSelector>
    {
    public:
        PlayerStop(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId,
            std::chrono::system_clock::time_point creationTimePoint = std::chrono::system_clock::now());

    private:
        void Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        game_entity_id_type _id;
        Eigen::Vector3d _newPosition;
    };
}