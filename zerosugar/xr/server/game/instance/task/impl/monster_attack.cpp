#include "monster_attack.h"

#include "zerosugar/xr/data/table/monster.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/component/monster_motion_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/task/impl/monster_attack_effect.h"

namespace zerosugar::xr::game_task
{
    MonsterAttack::MonsterAttack(game_entity_id_type entityId, const MonsterAttackContext& context, game_time_point_type creationTimePoint)
        : GameTaskParamT(creationTimePoint, context, MainTargetSelector(entityId))
    {
    }

    void MonsterAttack::Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type monster)
    {
        (void)parallelContext;

        const MonsterAttackContext& context = GetParam();
        
        MovementComponent& movementComponent = monster->GetComponent<MovementComponent>();
        const Eigen::Vector3d currentPos = movementComponent.GetPosition();

        if (context.animation->forwardMovement > 0)
        {
            _destPosition = movementComponent.StartForwardMovement(
                context.yaw,
                context.animation->forwardMovement,
                context.animation->duration
            );

            _destMovementDuration = context.animation->duration;
        }

        MonsterMotionComponent& motionComponent = monster->GetComponent<MonsterMotionComponent>();

        motionComponent.SetMotionId(context.skill->id);
        const int64_t counter = motionComponent.ChangeMotion(MotionType::Attack, context.animation->duration);

        for (const data::MonsterAnimation::Attack& attack : context.animation->attacks)
        {
            const auto delay = GetMilliFromGameSeconds(attack.applyTimePoint);

            std::stop_source source;

            Eigen::AlignedBox2d box;
            box.min() = Eigen::Vector2d(attack.minX, attack.minY);
            box.max() = Eigen::Vector2d(attack.maxX, attack.maxY);
            float yaw = static_cast<float>(movementComponent.GetYaw());

            Delay(delay).Then(parallelContext.GetExecutor(),
                [parallel = parallelContext.Hold(), token = source.get_token(), id = monster->GetId(), counter, currentPos, yaw, box]()
                {
                    if (token.stop_requested())
                    {
                        return;
                    }

                    MonsterAttackEffectContext context;
                    context.attackerId = id;
                    context.counter = counter;

                    parallel->SummitTask(std::make_unique<MonsterAttackEffect>(context, currentPos, yaw, box));
                });

            motionComponent.AddAttackEffectCancelToken(std::move(source));
        }
    }

    void MonsterAttack::OnComplete(GameExecutionSerial& serialContext)
    {
        GameSnapshotContainer& snapshotContainer = serialContext.GetSnapshotContainer();
        GameSnapshotController& controller = serialContext.GetSnapshotController();

        GameMonsterSnapshot* self = snapshotContainer.FindMonster(GetSelector<MainTargetSelector>().GetTargetId()[0]);
        assert(self);

        controller.ProcessMonsterAttack(*self, GetParam().animation->index,
            _destPosition, Eigen::Vector3d(0, GetParam().yaw, 0.0),
            _destMovementDuration);
    }
}
