#include "monster_spawn.h"

#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/ai_control_service.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_container.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/stat_component.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
#include "zerosugar/xr/server/game/instance/task/game_task_scheduler.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"

namespace zerosugar::xr::game_task
{
    MonsterSpawn::MonsterSpawn(const MonsterSpawnContext& spawnContext, game_time_point_type creationTimePoint)
        : GameTaskParamT(creationTimePoint, spawnContext, NullSelector{})
    {
    }

    MonsterSpawn::~MonsterSpawn()
    {
    }

    bool MonsterSpawn::ShouldPrepareBeforeScheduled() const
    {
        return true;
    }

    void MonsterSpawn::Prepare(GameExecutionSerial& serialContext, bool& quickExit)
    {
        quickExit = false;

        constexpr const char* behaviorTreeName = "monster_aggressive";

        _entityId = serialContext.PublishEntityId(GameEntityType::Monster);
        _aiController = serialContext.GetAIControlService().CreateAIController(_entityId, behaviorTreeName);

        serialContext.GetTaskScheduler().AddController(_aiController->GetControllerId());
        serialContext.GetTaskScheduler().AddEntity(_entityId);
    }

    void MonsterSpawn::Execute(GameExecutionParallel& parallelContext, NullSelector::target_type)
    {
        auto entity = std::make_shared<GameEntity>();
        entity->SetId(_entityId);
        entity->SetController(_aiController);

        const MonsterSpawnContext& context = GetParam();
        {
            auto movementComponent = std::make_unique<MovementComponent>();
            movementComponent->SetPosition(Eigen::Vector3d(context.x, context.y, context.z));

            entity->AddComponent(std::move(movementComponent));
        }
        {
            auto statComponent = std::make_unique<StatComponent>();
            statComponent->SetMaxHP(StatValue(100.0));
            statComponent->SetHP(StatValue(100.0));

            entity->AddComponent(std::move(statComponent));
        }

        [[maybe_unused]]
        const bool added = parallelContext.GetEntityContainer().Add(entity);
        assert(added);

        _snapshot = std::make_unique<GameMonsterSnapshot>(*_aiController, _entityId, GetParam().dataId);
        _snapshot->Initialize(*entity);
    }

    void MonsterSpawn::OnComplete(GameExecutionSerial& serialContext)
    {
        const GameMonsterSnapshot* snapshot = _snapshot.get();
        assert(snapshot);

        [[maybe_unused]]
        const bool added = serialContext.GetSnapshotContainer().Add(std::move(_snapshot));
        assert(added);

        serialContext.GetSnapshotController().ProcessMonsterSpawn(*snapshot);

        _aiController->Start();
    }
}
