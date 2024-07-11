#include "monster_attack_effect.h"

#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/component/monster_motion_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/player_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/stat_component.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"

namespace zerosugar::xr::game_task
{
    MonsterAttackEffect::MonsterAttackEffect(const MonsterAttackEffectContext& context,
        const Eigen::Vector3d& center, float yaw, const Eigen::AlignedBox2d& targetingBox, game_time_point_type creationTimePoint)
        : GameTaskParamT(creationTimePoint, context,
            MainTargetSelector(context.attackerId),
            BoxSkillTargetSelector(center, targetingBox, yaw, GameEntityType::Player))
    {
    }

    void MonsterAttackEffect::Execute(GameExecutionParallel& parallelContext,
        MainTargetSelector::target_type monster,
        BoxSkillTargetSelector::target_type targets)
    {
        (void)parallelContext;

        const MonsterAttackEffectContext& context = GetParam();
        MonsterMotionComponent& motionComponent = monster->GetComponent<MonsterMotionComponent>();

        if (context.counter != motionComponent.GetCounter())
        {
            return;
        }

        _attackId = motionComponent.GetMotionId();
        _results.reserve(targets.size());

        for (PtrNotNull<GameEntity> target : targets)
        {
            if (target->GetComponent<PlayerComponent>().IsDodgeState())
            {
                continue;
            }

            StatComponent& statComponent = target->GetComponent<StatComponent>();

            const StatValue currentHP = statComponent.GetHP();
            const StatValue resultHP(currentHP.As<float>() - 10.f);

            statComponent.SetHP(resultHP);
            _results.emplace_back(target->GetId(), resultHP.As<float>());
        }
    }

    void MonsterAttackEffect::OnComplete(GameExecutionSerial& serialContext)
    {
        if (!_attackId.has_value() || _results.empty())
        {
            return;
        }

        const game_entity_id_type selfId = GetSelector<MainTargetSelector>().GetTargetId()[0];
        GameSnapshotController& controller = serialContext.GetSnapshotController();

        for (const auto& [attacked, hp] : _results)
        {
            controller.ProcessMonsterAttackEffect(selfId, *_attackId, attacked, hp);
        }
    }
}
