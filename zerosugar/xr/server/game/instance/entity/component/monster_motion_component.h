#pragma once
#include "zerosugar/xr/server/game/instance/game_type.h"
#include "zerosugar/xr/server/game/instance/entity/component/game_component.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_parallel.h"

namespace zerosugar::xr
{
    enum class MotionType
    {
        Idle,
        Move,
        Attack,
        Dead,
    };

    class MonsterMotionComponent final : public GameComponent
    {
    public:
        bool IsRunning(MotionType type, game_time_point_type current = GameExecutionParallel::GetBaseTimePoint()) const;

        auto ChangeMotion(MotionType type, double duration, game_time_point_type current = GameExecutionParallel::GetBaseTimePoint())
            -> int64_t;

        void AddAttackEffectCancelToken(std::stop_source token);
        void CancelAttackEffects();

        auto GetCurrentMotionType() const -> MotionType;
        auto GetCounter() const -> int64_t;
        auto GetMotionId() const -> int32_t;

        void SetMotionId(int32_t motionId);

    private:
        MotionType _type = MotionType::Attack;
        int64_t _counter = 0;

        game_time_point_type _startTimePoint = {};
        game_time_point_type _endTimePoint = {};

        int32_t _motionId = 0;
        std::vector<std::stop_source> _attackEffectCancelTokens;
    };
}
