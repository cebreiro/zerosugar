#pragma once
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    class PlayerLocalChat final : public GameTaskBaseParamT<IPacket, network::game::cs::Chat, NullSelector>
    {
    public:
        PlayerLocalChat(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId,
            game_time_point_type creationTimePoint = game_clock_type::now());

    private:
        bool ShouldPrepareBeforeScheduled() const override;
        void Prepare(GameExecutionSerial& serialContext, bool& quickExit) override;

        void Execute(GameExecutionParallel& parallelContext, NullSelector::target_type) override;;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        game_entity_id_type _id;
    };
}
