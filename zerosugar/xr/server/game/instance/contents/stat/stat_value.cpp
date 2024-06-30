#include "stat_value.h"

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
        const StatValue::value_type value = lhs._value + rhs._value;
        assert(!std::isnan(value));

        return StatValue(value);
    }

    auto operator-(StatValue lhs, StatValue rhs) -> StatValue
    {
        const StatValue::value_type value = lhs._value - rhs._value;
        assert(!std::isnan(value));

        return StatValue(value);
    }

    auto operator*(StatValue lhs, StatValue rhs) -> StatValue
    {
        const StatValue::value_type value = lhs._value * rhs._value;
        assert(!std::isnan(value));

        return StatValue(value);
    }

    auto operator/(StatValue lhs, StatValue rhs) -> StatValue
    {
        const StatValue::value_type value = lhs._value / rhs._value;
        assert(!std::isnan(value));

        return StatValue(value);
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

    auto StatValue::operator*=(StatValue other) -> StatValue&
    {
        (*this) = (*this) * other;

        return *this;
    }

    auto StatValue::operator/=(StatValue other) -> StatValue&
    {
        (*this) = (*this) / other;

        return *this;
    }

    auto StatValue::Zero() -> StatValue
    {
        return StatValue(0.0);
    }
}
