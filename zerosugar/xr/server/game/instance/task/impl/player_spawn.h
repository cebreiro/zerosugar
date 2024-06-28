#pragma once
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    class PlayerSpawn final : public GameTaskParamT<SharedPtrNotNull<GameEntity>, NullSelector>
    {
    public:
        explicit PlayerSpawn(SharedPtrNotNull<GameEntity> player,
            std::chrono::system_clock::time_point creationTimePoint = std::chrono::system_clock::now());

    private:
        bool ShouldPrepareBeforeScheduled() const override;
        void Prepare(GameExecutionSerial& serialContext) override;

        void Execute(GameExecutionParallel& parallelContext, NullSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;
    };
}
