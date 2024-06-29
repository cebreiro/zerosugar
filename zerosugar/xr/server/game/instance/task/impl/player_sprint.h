#pragma once
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    class PlayerSprint final : public GameTaskBaseParamT<IPacket, network::game::cs::SprintPlayer, MainTargetSelector>
    {
    public:
        PlayerSprint(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId,
            std::chrono::system_clock::time_point creationTimePoint = std::chrono::system_clock::now());

    private:
        void Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        game_entity_id_type _id;
    };
}
