#include "monster_despawn.h"

#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/ai_control_service.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_container.h"
#include "zerosugar/xr/server/game/instance/entity/component/monster_component.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
#include "zerosugar/xr/server/game/instance/task/game_task_scheduler.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_parallel.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"

namespace zerosugar::xr::game_task
{
    MonsterDespawn::MonsterDespawn(MonsterDespawnContext context, game_controller_id_type controllerId,
        game_entity_id_type id, game_time_point_type creationTimePoint)
        : GameTaskParamT(creationTimePoint, std::move(context), MainTargetSelector(id))
        , _entityId(id)
        , _controllerId(controllerId)
    {
    }

    bool MonsterDespawn::ShouldPrepareBeforeScheduled() const
    {
        return true;
    }

    void MonsterDespawn::Prepare(GameExecutionSerial& serialContext, bool& quickExit)
    {
        quickExit = false;

        if (AIController* controller = serialContext.GetAIControlService().FindAIController(_entityId); controller)
        {
            controller->Shutdown();

            assert(controller->GetId() == _controllerId);
        }

        serialContext.GetTaskScheduler().RemoveController(_controllerId);
        serialContext.GetTaskScheduler().RemoveEntity(_entityId);
    }

    void MonsterDespawn::Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type entity)
    {
        _spawnerId = entity->GetComponent<MonsterComponent>().GetSpawnerId();

        [[maybe_unused]]
        const bool removed = parallelContext.GetEntityContainer().Remove(entity->GetId());
        assert(removed);
    }

    void MonsterDespawn::OnComplete(GameExecutionSerial& serialContext)
    {
        serialContext.GetSnapshotController().ProcessMonsterDespawn(_entityId);

        if (_spawnerId)
        {
            if (IGameController* controller = serialContext.GetSnapshotContainer().FindController(*_spawnerId); controller)
            {
                network::game::sc::SpawnerMonsterDead event;

                controller->Notify(event);
            }
        }
    }
}
