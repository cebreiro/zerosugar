#include "player_attack_effect_apply.h"

#include "zerosugar/xr/data/table/monster.h"
#include "zerosugar/xr/server/game/instance/controller/game_controller_interface.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_container.h"
#include "zerosugar/xr/server/game/instance/entity/component/monster_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/monster_motion_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/stat_component.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
#include "zerosugar/xr/server/game/instance/task/game_task_scheduler.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/task/impl/monster_despawn.h"

namespace zerosugar::xr::game_task
{
    PlayerAttackEffectApply::PlayerAttackEffectApply(UniquePtrNotNull<network::game::cs::ApplyPlayerAttack> param, game_entity_id_type playerId,
        std::span<const game_entity_id_type> targetIds, game_time_point_type creationTimePoint)
        : GameTaskBaseParamT(creationTimePoint, std::move(param), MainTargetSelector(playerId), PlayerAttackEffectTargetSelector(playerId, targetIds))
    {
    }

    void PlayerAttackEffectApply::Execute(GameExecutionParallel& parallelContext,
        MainTargetSelector::target_type player, PlayerAttackEffectTargetSelector::target_type targets)
    {
        constexpr double damage = 60.0;
        (void)player;

        for (const PtrNotNull<GameEntity>& target : targets)
        {
            StatComponent& targetStatComponent = target->GetComponent<StatComponent>();
            double currentHP = targetStatComponent.GetHP().Get();

            if (currentHP <= 0.0)
            {
                assert(false);

                continue;
            }

            const double newHP = std::max(0.0, currentHP - damage);
            targetStatComponent.SetHP(StatValue(newHP));

            if (newHP > 0.0)
            {
                _aliveMonsters.emplace_back(target->GetId(), static_cast<float>(newHP));
            }
            else
            {
                MonsterMotionComponent& motionComponent = target->GetComponent<MonsterMotionComponent>();
                motionComponent.CancelAttackEffects();

                MonsterComponent& monsterComponent = target->GetComponent<MonsterComponent>();
                const MonsterData& data = monsterComponent.GetData();
                const data::MonsterAnimation::Value& animation = data.GetDeadAnimation();

                _deadMonsters.emplace_back(DeadContext{
                    .entityId = target->GetId(),
                    .controllerId = target->GetController().GetControllerId(),
                    .animationDuration = animation.duration,
                    .spawnerId = monsterComponent.GetSpawnerId(),
                    });

                [[maybe_unused]]
                const bool removed = parallelContext.GetEntityContainer().Remove(target->GetId());
                assert(removed);
            }
        }
    }

    void PlayerAttackEffectApply::OnComplete(GameExecutionSerial& serialContext)
    {
        auto selfId = GetSelector<MainTargetSelector>().GetTargetId()[0];

        GameSnapshotController& snapshotController = serialContext.GetSnapshotController();
        GameTaskScheduler& taskScheduler = serialContext.GetTaskScheduler();

        for (const auto& [id, controllerId, duration, spawnerId] : _deadMonsters)
        {
            taskScheduler.RemoveController(controllerId);
            taskScheduler.RemoveEntity(id);

            snapshotController.ProcessMonsterDespawn(id, duration, spawnerId);
        }

        snapshotController.ProcessPlayerAttackEffect(selfId, _aliveMonsters);


    }
}
