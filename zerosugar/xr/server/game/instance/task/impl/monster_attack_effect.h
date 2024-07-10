#pragma once
#include <Eigen/Geometry>
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    struct MonsterAttackEffectContext
    {
        game_entity_id_type attackerId;
        int64_t counter = 0;
    };

    class MonsterAttackEffect final : public GameTaskParamT<MonsterAttackEffectContext, MainTargetSelector, BoxSkillTargetSelector>
    {
    public:
        MonsterAttackEffect(const MonsterAttackEffectContext& context, const Eigen::Vector3d& center, float yaw, const Eigen::AlignedBox2d& targetingBox,
            game_time_point_type creationTimePoint = game_clock_type::now());

    private:
        void Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type, BoxSkillTargetSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        std::optional<int32_t> _attackId = std::nullopt;
        std::vector<std::pair<game_entity_id_type, float>> _results;
    };
}
