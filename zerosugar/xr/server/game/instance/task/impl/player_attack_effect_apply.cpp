#include "player_attack_effect_apply.h"

#include "zerosugar/xr/data/table/monster.h"
#include "zerosugar/xr/server/game/instance/controller/game_controller_interface.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/component/monster_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/stat_component.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
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
        (void)parallelContext;

        constexpr double damage = 60.0;
        (void)player;

        for (const PtrNotNull<GameEntity>& target : targets)
        {
            const MonsterComponent& monsterComponent = target->GetComponent<MonsterComponent>();

            StatComponent& targetStatComponent = target->GetComponent<StatComponent>();
            double hp = targetStatComponent.GetHP().Get();

            if (hp <= 0.0)
            {
                continue;
            }

            const double resultHP = std::max(0.0, hp - damage);
            targetStatComponent.SetHP(StatValue(resultHP));

            if (resultHP <= 0.0)
            {
                targetStatComponent.SetHPRegen(false);

                const MonsterData& data = monsterComponent.GetData();
                const data::MonsterAnimation::Value& animation = data.GetDeadAnimation();
                const auto delay = GetMilliFromGameSeconds(animation.duration);

                Delay(delay).Then(parallelContext.GetExecutor(),
                    [context = parallelContext.Hold(), controllerId = target->GetController().GetControllerId(), id = target->GetId()]()
                    {
                        auto task = std::make_unique<game_task::MonsterDespawn>(MonsterDespawnContext{}, controllerId, id);

                        context->SummitTask(std::move(task), std::nullopt);
                    });

                // change animation... item drop
            }

            _results.emplace_back(target->GetId(), static_cast<float>(resultHP));
        }
    }

    void PlayerAttackEffectApply::OnComplete(GameExecutionSerial& serialContext)
    {
        auto selfId = GetSelector<MainTargetSelector>().GetTargetId()[0];

        serialContext.GetSnapshotController().ProcessPlayerAttackEffect(selfId, _results);
    }
}
