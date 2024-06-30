#pragma once
#include "zerosugar/xr/server/game/instance/game_constants.h"
#include "zerosugar/xr/server/game/instance/contents/stat/stat_value.h"

namespace zerosugar::xr
{
    class RegenStat
    {
    public:
        RegenStat() = default;
        RegenStat(StatValue value, StatValue regenValue, StatValue min, StatValue max);

        auto GetValue(game_time_point_type now) const -> StatValue;
        auto GetMaxValue() const -> StatValue;

        void ChangeRegenValue(StatValue regenValue, game_time_point_type now);
        void SetValue(StatValue value, game_time_point_type now);

    private:
        void Update(game_time_point_type now);

    private:
        StatValue _value;
        StatValue _regenValue;
        StatValue _min;
        StatValue _max;

        mutable game_time_point_type _lastUpdateTimePoint = game_constant::null_time_point;
    };
}
