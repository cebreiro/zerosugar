#pragma once
#include "zerosugar/xr/data/provider/map_data.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr
{
    class AIController;
    class GameSpawnerSnapshot;
}

namespace zerosugar::xr::game_task
{
    class SpawnerInstall final : public GameTaskParamT<PtrNotNull<const data::MonsterSpawner>, NullSelector>
    {
    public:
        explicit SpawnerInstall(PtrNotNull<const data::MonsterSpawner> data,
            game_time_point_type creationTimePoint = game_clock_type::now());
        ~SpawnerInstall();

    private:
        bool ShouldPrepareBeforeScheduled() const override;
        void Prepare(GameExecutionSerial& serialContext, bool& quickExit) override;

        void Execute(GameExecutionParallel& parallelContext, NullSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        game_entity_id_type _entityId;
        SharedPtrNotNull<AIController> _aiController;
        UniquePtrNotNull<GameSpawnerSnapshot> _snapshot;
    };
}
