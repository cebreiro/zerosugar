#pragma once
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    class PlayerActivate final : public GameTaskT<NullSelector>
    {
    public:
        explicit PlayerActivate(game_entity_id_type entityId,
            game_time_point_type creationTimePoint = game_clock_type::now());

    private:
        bool ShouldPrepareBeforeScheduled() const override;
        void Prepare(GameExecutionSerial& serialContext, bool& quickExit) override;

        void Execute(GameExecutionParallel& parallelContext, NullSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        game_entity_id_type _entityId;
    };
}
