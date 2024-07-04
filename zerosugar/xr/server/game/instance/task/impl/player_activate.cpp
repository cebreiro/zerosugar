#include "player_activate.h"

#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"

namespace zerosugar::xr::game_task
{
    PlayerActivate::PlayerActivate(game_entity_id_type entityId, game_time_point_type creationTimePoint)
        : GameTaskT(creationTimePoint, NullSelector())
        , _entityId(entityId)
    {
    }

    bool PlayerActivate::ShouldPrepareBeforeScheduled() const
    {
        return true;
    }

    void PlayerActivate::Prepare(GameExecutionSerial& serialContext, bool& quickExit)
    {
        quickExit = true;

        serialContext.GetSnapshotController().ProcessPlayerActivate(_entityId);
    }

    void PlayerActivate::Execute(GameExecutionParallel& parallelContext, NullSelector::target_type)
    {
        (void)parallelContext;
    }

    void PlayerActivate::OnComplete(GameExecutionSerial& serialContext)
    {
        (void)serialContext;
    }
}
