#pragma once
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    class PlayerPingPong final : public GameTaskBaseParamT<IPacket, network::game::cs::Ping, MainTargetSelector>
    {
    public:
        PlayerPingPong(UniquePtrNotNull<network::game::cs::Ping> param, game_entity_id_type targetId,
            game_time_point_type creationTimePoint = game_clock_type::now());

    private:
        void Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type player) override;
        void OnComplete(GameExecutionSerial& serialContext) override;
    };
}
