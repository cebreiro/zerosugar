#pragma once
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr
{
    class AIController;
    class GameMonsterSnapshot;
}

namespace zerosugar::xr::game_task
{
    struct MonsterSpawnContext
    {
        int32_t dataId = 0;
        float x = 0.f;
        float y = 0.f;
        float z = 0.f;
    };

    class MonsterSpawn final : public GameTaskParamT<MonsterSpawnContext, NullSelector>
    {
    public:
        explicit MonsterSpawn(const MonsterSpawnContext& spawnContext,
            game_time_point_type creationTimePoint = game_clock_type::now());
        ~MonsterSpawn();

    private:
        bool ShouldPrepareBeforeScheduled() const override;
        void Prepare(GameExecutionSerial& serialContext, bool& quickExit) override;

        void Execute(GameExecutionParallel& parallelContext, NullSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        game_entity_id_type _entityId;
        SharedPtrNotNull<AIController> _aiController;
        UniquePtrNotNull<GameMonsterSnapshot> _snapshot;
    };
}
