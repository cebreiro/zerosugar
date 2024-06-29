#include "player_spawn.h"

#include "zerosugar/xr/server/game/controller/game_entity_controller_interface.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_container.h"
#include "zerosugar/xr/server/game/instance/task/game_task_scheduler.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_parallel.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/view/game_player_view_model.h"
#include "zerosugar/xr/server/game/instance/view/game_view_controller.h"
#include "zerosugar/xr/server/game/instance/view/game_view_model_container.h"

namespace zerosugar::xr::game_task
{
    PlayerSpawn::PlayerSpawn(SharedPtrNotNull<GameEntity> player, std::chrono::system_clock::time_point creationTimePoint)
        : GameTaskParamT(creationTimePoint, std::move(player), NullSelector{})
    {
    }

    bool PlayerSpawn::ShouldPrepareBeforeScheduled() const
    {
        return true;
    }

    void PlayerSpawn::Prepare(GameExecutionSerial& serialContext, bool& quickExit)
    {
        quickExit = false;

        const int64_t controllerId = GetParam()->GetController().GetControllerId();
        const game_entity_id_type entityId = GetParam()->GetId();

        serialContext.GetTaskScheduler().AddProcess(controllerId);
        serialContext.GetTaskScheduler().AddResource(entityId.Unwrap());
    }

    void PlayerSpawn::Execute(GameExecutionParallel& parallelContext, NullSelector::target_type)
    {
        [[maybe_unused]]
        const bool added = parallelContext.GetEntityContainer().Add(GetParam());
        assert(added);
    }

    void PlayerSpawn::OnComplete(GameExecutionSerial& serialContext)
    {
        auto playerView = std::make_unique<GamePlayerViewModel>(GetParam()->GetController());
        playerView->Initialize(*GetParam());

        [[maybe_unused]]
        const bool result = serialContext.GetViewModelContainer().Add(std::move(playerView));
        assert(result);

        serialContext.GetViewController().ProcessPlayerSpawn(*GetParam());
    }
}
