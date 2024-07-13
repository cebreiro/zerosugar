#pragma once
#include "zerosugar/xr/server/game/instance/controller/game_controller_id.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    struct MonsterDespawnContext
    {
    };

    class MonsterDespawn final : public GameTaskParamT<MonsterDespawnContext, MainTargetSelector>
    {
    public:
        MonsterDespawn(MonsterDespawnContext context, game_controller_id_type controllerId, game_entity_id_type id,
            game_time_point_type creationTimePoint = game_clock_type::now());

    private:
        bool ShouldPrepareBeforeScheduled() const override;;
        void Prepare(GameExecutionSerial& serialContext, bool& quickExit) override;

        void Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        game_entity_id_type _entityId;
        game_controller_id_type _controllerId;

        std::optional<game_entity_id_type> _spawnerId = std::nullopt;
    };
}
