#include "regen_stat.h"

namespace zerosugar::xr
{
    RegenStat::RegenStat(StatValue value, StatValue regenValue, StatValue min, StatValue max)
        : _value(value)
        , _regenValue(regenValue)
        , _min(min)
        , _max(max)
    {
    }

    auto RegenStat::GetValue(game_time_point_type now) const-> StatValue
    {
        assert(now >= _lastUpdateTimePoint);

        if (_lastUpdateTimePoint == game_constant::null_time_point)
        {
            _lastUpdateTimePoint = now;

            return _value;
        }

        if (_lastUpdateTimePoint == now)
        {
            return _value;
        }

        if (_value.Get() >= _max.Get())
        {
            _lastUpdateTimePoint = now;

            return _value;
        }

        const_cast<RegenStat*>(this)->Update(now);

        return _value;
    }

    auto RegenStat::GetMaxValue() const -> StatValue
    {
        return _max;
    }

    void RegenStat::ChangeRegenValue(StatValue regenValue, game_time_point_type now)
    {
        Update(now);

        _regenValue = regenValue;
    }

    void RegenStat::SetValue(StatValue value, game_time_point_type now)
    {
        _lastUpdateTimePoint = now;

        _value = value;
    }

    void RegenStat::Update(game_time_point_type now)
    {
        const auto interval = std::chrono::duration_cast<std::chrono::milliseconds>((now - _lastUpdateTimePoint));
        _lastUpdateTimePoint = now;

        const StatValue::value_type expected = _value.Get() + (static_cast<StatValue::value_type>(interval.count()) * _regenValue.Get());

        _value = StatValue(std::clamp(expected, _min.Get(), _max.Get()));
    }
}
