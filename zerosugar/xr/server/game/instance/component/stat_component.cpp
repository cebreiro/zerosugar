#include "stat_component.h"

namespace zerosugar::xr
{
    StatValue::StatValue(value_type value)
        : _value(value)
    {
    }

    auto StatValue::Get() const -> value_type
    {
        return _value;
    }

    auto operator-(StatValue value) -> StatValue
    {
        return StatValue(-value.Get());
    }

    auto operator+(StatValue lhs, StatValue rhs) -> StatValue
    {
        return StatValue(lhs._value + rhs._value);
    }

    auto operator-(StatValue lhs, StatValue rhs) -> StatValue
    {
        return StatValue(lhs._value - rhs._value);
    }

    auto operator*(StatValue lhs, StatValue rhs) -> StatValue
    {
        return StatValue(lhs._value * rhs._value);
    }

    auto operator/(StatValue lhs, StatValue rhs) -> StatValue
    {
        return StatValue(lhs._value / rhs._value);
    }

    auto StatValue::operator+=(StatValue other) -> StatValue&
    {
        (*this) = (*this) + other;

        return *this;
    }

    auto StatValue::operator-=(StatValue other) -> StatValue&
    {
        (*this) = (*this) - other;

        return *this;
    }

    RegenStat::RegenStat(StatValue value, StatValue regenValue, StatValue min, StatValue max)
        : _value(value)
        , _regenValue(regenValue)
        , _min(min)
        , _max(max)
    {
    }

    auto RegenStat::GetValue(std::chrono::system_clock::time_point now) const-> StatValue
    {
        assert(now >= _lastUpdateTimePoint);

        if (_lastUpdateTimePoint == null_time_point)
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

    void RegenStat::ChangeRegenValue(StatValue regenValue, std::chrono::system_clock::time_point now)
    {
        Update(now);

        _regenValue = regenValue;
    }

    void RegenStat::SetValue(StatValue value, std::chrono::system_clock::time_point now)
    {
        _lastUpdateTimePoint = now;

        _value = value;
    }

    void RegenStat::Update(std::chrono::system_clock::time_point now)
    {
        auto interval = std::chrono::duration_cast<std::chrono::milliseconds>((now - _lastUpdateTimePoint));
        _lastUpdateTimePoint = now;

        const StatValue::value_type expected = _value.Get() + (static_cast<StatValue::value_type>(interval.count()) * _regenValue.Get());

        _value = StatValue(std::clamp(expected, _min.Get(), _max.Get()));
    }

    void FixedStat::AddBaseStat(StatValue value)
    {
        _base += value;

        _dirty = true;
    }

    void FixedStat::AddItemStat(StatValue value)
    {
        _item += value;

        _dirty = true;
    }

    auto FixedStat::GetBaseStat() const -> StatValue
    {
        return _base;
    }

    auto FixedStat::GetItemStat() const -> StatValue
    {
        return _item;
    }

    auto FixedStat::GetFinalValue() const -> StatValue
    {
        if (_dirty)
        {
            _dirty = false;

            _finalValue = Recalculate();
        }

        return _finalValue;
    }

    auto FixedStat::Recalculate() const -> StatValue
    {
        return _item + _base;
    }

    StatComponent::StatComponent()
    {
    }

    void StatComponent::AddItemStat(StatType type, StatValue value)
    {
        GetFixedStat(type).AddItemStat(value);
    }

    void StatComponent::SubItemStat(StatType type, StatValue value)
    {
        GetFixedStat(type).AddItemStat(-value);
    }

    auto StatComponent::GetHP(std::chrono::system_clock::time_point now) const -> StatValue
    {
        return _hp.GetValue(now);
    }

    auto StatComponent::GetMP(std::chrono::system_clock::time_point now) const -> StatValue
    {
        return _mp.GetValue(now);
    }

    auto StatComponent::GetStamina(std::chrono::system_clock::time_point now) const -> StatValue
    {
        return _stamina.GetValue(now);
    }

    auto StatComponent::Get(StatType type) const -> StatValue
    {
        return GetFixedStat(type).GetFinalValue();
    }

    auto StatComponent::GetMaxHP() const -> StatValue
    {
        return _hp.GetMaxValue();
    }

    auto StatComponent::GetMaxMP() const -> StatValue
    {
        return _mp.GetMaxValue();
    }

    auto StatComponent::GetMaxStamina() const -> StatValue
    {
        return _stamina.GetMaxValue();
    }

    void StatComponent::SetHP(StatValue value, std::chrono::system_clock::time_point now)
    {
        _hp.SetValue(value, now);
    }

    void StatComponent::SetMP(StatValue value, std::chrono::system_clock::time_point now)
    {
        _mp.SetValue(value, now);
    }

    auto StatComponent::GetFixedStat(StatType type) -> FixedStat&
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_stats));

        return _stats[index];
    }

    auto StatComponent::GetFixedStat(StatType type) const -> const FixedStat&
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_stats));

        return _stats[index];
    }
}
