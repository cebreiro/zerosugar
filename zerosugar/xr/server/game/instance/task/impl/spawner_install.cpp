#include "spawner_install.h"

#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/ai_control_service.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_container.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_spawner_snapshot.h"
#include "zerosugar/xr/server/game/instance/task/game_task_scheduler.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_parallel.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"

namespace zerosugar::xr::game_task
{
    SpawnerInstall::SpawnerInstall(PtrNotNull<const data::MonsterSpawner> data, game_time_point_type creationTimePoint)
        : GameTaskParamT(creationTimePoint, data, NullSelector{})
    {
    }

    SpawnerInstall::~SpawnerInstall()
    {
    }

    bool SpawnerInstall::ShouldPrepareBeforeScheduled() const
    {
        return true;
    }

    void SpawnerInstall::Prepare(GameExecutionSerial& serialContext, bool& quickExit)
    {
        quickExit = false;

        _entityId = serialContext.PublishEntityId(GameEntityType::Spawner);

        constexpr const char* behaviorTreeName = "spawner";

        _aiController = serialContext.GetAIControlService().CreateAIController(_entityId, behaviorTreeName);
        _aiController->GetBlackBoard().Insert<PtrNotNull<const data::MonsterSpawner>>("param", GetParam());

        serialContext.GetTaskScheduler().AddController(_aiController->GetControllerId());
        serialContext.GetTaskScheduler().AddEntity(_entityId);
    }

    void SpawnerInstall::Execute(GameExecutionParallel& parallelContext, NullSelector::target_type)
    {
        const data::MonsterSpawner& data = *GetParam();

        auto entity = std::make_shared<GameEntity>();
        entity->SetId(_entityId);
        entity->SetController(_aiController);

        const Eigen::Vector3d position(data.x, data.y, data.z);

        auto movementComponent = std::make_unique<MovementComponent>();
        movementComponent->SetPosition(position);

        entity->AddComponent(std::move(movementComponent));

        [[maybe_unused]]
        const bool added = parallelContext.GetEntityContainer().Add(entity);
        assert(added);

        _snapshot = std::make_unique<GameSpawnerSnapshot>(*_aiController, _entityId);
        _snapshot->Initialize(*entity);
    }

    void SpawnerInstall::OnComplete(GameExecutionSerial& serialContext)
    {
        const GameSpawnerSnapshot* snapshot = _snapshot.get();
        assert(snapshot);

        [[maybe_unused]]
        const bool added = serialContext.GetSnapshotContainer().Add(std::move(_snapshot));
        assert(added);

        serialContext.GetSnapshotController().ProcessSpawnerAdd(*snapshot);

        _aiController->Start();
    }
}
