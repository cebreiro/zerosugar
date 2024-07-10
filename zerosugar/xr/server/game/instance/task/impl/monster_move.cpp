#include "monster_move.h"

#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"

namespace zerosugar::xr::game_task
{
    MonsterMove::MonsterMove(const MonsterMoveContext& context, game_time_point_type creationTimePoint)
        : GameTaskParamT(creationTimePoint, context, MainTargetSelector(context.monsterId))
    {
    }

    void MonsterMove::Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type entity)
    {
        (void)parallelContext;

        MovementComponent& movementComponent = entity->GetComponent<MovementComponent>();

        const MonsterMoveContext& context = GetParam();

        // TODO: check movable

        movementComponent.UpdatePointMovement(context.position, context.timePoint, context.next);

        _position = movementComponent.GetPosition();
        _yaw = static_cast<float>(movementComponent.GetYaw());
    }

    void MonsterMove::OnComplete(GameExecutionSerial& serialContext)
    {
        GameMonsterSnapshot* monster = serialContext.GetSnapshotContainer().FindMonster(GetParam().monsterId);
        assert(monster);

        serialContext.GetSnapshotController().ProcessMonsterMove(*monster, _position, _yaw);
    }
}
