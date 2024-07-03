#include "player_despawn.h"

#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
#include "zerosugar/xr/server/game/instance/task/game_task_scheduler.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_parallel.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"

namespace zerosugar::xr::game_task
{
    PlayerDepsawn::PlayerDepsawn(Promise<void> completionToken, game_controller_id_type controllerId,
        game_entity_id_type entityId, game_time_point_type creationTimePoint)
        : GameTaskParamT(creationTimePoint, std::move(completionToken), MainTargetSelector(entityId))
        , _controllerId(controllerId)
        , _entityId(entityId)
    {
    }

    bool PlayerDepsawn::ShouldPrepareBeforeScheduled() const
    {
        return true;
    }

    void PlayerDepsawn::Prepare(GameExecutionSerial& serialContext, bool& quickExit)
    {
        quickExit = false;

        serialContext.GetTaskScheduler().RemoveController(_controllerId);
        serialContext.GetTaskScheduler().RemoveEntity(_entityId);
    }

    void PlayerDepsawn::Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type entity)
    {
        [[maybe_unused]]
        const bool removed = parallelContext.GetEntityContainer().Remove(entity->GetId());
        assert(removed);
    }

    void PlayerDepsawn::OnComplete(GameExecutionSerial& serialContext)
    {
        Promise<void>& completionToken = MutableParam();
        completionToken.Set();

        serialContext.GetSnapshotController().ProcessPlayerDespawn(_entityId);
    }
}
