#pragma once
#include "zerosugar/xr/server/game/instance/controller/game_controller_id.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    class PlayerDepsawn final : public GameTaskParamT<Promise<void>, MainTargetSelector>
    {
    public:
        PlayerDepsawn(Promise<void> completionToken, game_controller_id_type controllerId, game_entity_id_type id,
            game_time_point_type creationTimePoint = game_clock_type::now());

    private:
        bool ShouldPrepareBeforeScheduled() const override;;
        void Prepare(GameExecutionSerial& serialContext, bool& quickExit) override;

        void Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        game_controller_id_type _controllerId;
        game_entity_id_type _entityId;
    };
}
