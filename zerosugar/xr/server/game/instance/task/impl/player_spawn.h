#pragma once
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    class PlayerSpawn final : public GameTaskParamT<SharedPtrNotNull<GameEntity>, NullSelector>
    {
    public:
        explicit PlayerSpawn(SharedPtrNotNull<GameEntity> player,
            game_time_point_type creationTimePoint = game_clock_type::now());

    private:
        bool ShouldPrepareBeforeScheduled() const override;
        void Prepare(GameExecutionSerial& serialContext, bool& quickExit) override;

        void Execute(GameExecutionParallel& parallelContext, NullSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        static void ConfigureStat(GameEntity& entity);
    };
}
