#pragma once
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    struct MonsterMoveContext
    {
        game_entity_id_type monsterId;

        Eigen::Vector3d position = {};
        game_time_point_type timePoint;

        std::optional<std::pair<Eigen::Vector3d, game_time_point_type>> next = std::nullopt;
    };

    class MonsterMove final : public GameTaskParamT<MonsterMoveContext, MainTargetSelector>
    {
    public:
        explicit MonsterMove(const MonsterMoveContext& context,
            game_time_point_type creationTimePoint = game_clock_type::now());

    private:
        void Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        Eigen::Vector3d _position;
        float _yaw = 0.f;
    };
}
