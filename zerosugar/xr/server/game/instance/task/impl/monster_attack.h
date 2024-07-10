#pragma once
#include "zerosugar/xr/data/table/monster.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    struct MonsterAttackContext
    {
        float yaw = 0.f;
        PtrNotNull<const data::MonsterSkill> skill = nullptr;
        PtrNotNull<const data::MonsterAnimation::Value> animation = nullptr;
    };

    class MonsterAttack final : public GameTaskParamT<MonsterAttackContext, MainTargetSelector>
    {
    public:
        MonsterAttack(game_entity_id_type entityId, const MonsterAttackContext& context,
            game_time_point_type creationTimePoint = game_clock_type::now());

    private:
        void Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        std::optional<Eigen::Vector3d> _destPosition = std::nullopt;
        double _destMovementDuration = 0.0;
    };
}
