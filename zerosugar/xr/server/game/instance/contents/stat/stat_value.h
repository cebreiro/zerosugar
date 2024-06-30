#pragma once
#include "zerosugar/xr/server/game/instance/contents/stat/stat_value.h"

namespace zerosugar::xr
{
    class StatValue
    {
    public:
        using value_type = double;

    public:
        StatValue() = default;
        explicit StatValue(value_type value);

        template <typename T> requires std::integral<T> || std::floating_point<T>
        auto As() const -> T
        {
            return static_cast<T>(_value);
        }

        auto Get() const -> value_type;

        friend auto operator-(StatValue value) -> StatValue;
        friend auto operator+(StatValue lhs, StatValue rhs) -> StatValue;
        friend auto operator-(StatValue lhs, StatValue rhs) -> StatValue;
        friend auto operator*(StatValue lhs, StatValue rhs) -> StatValue;
        friend auto operator/(StatValue lhs, StatValue rhs) -> StatValue;

        auto operator+=(StatValue other) -> StatValue&;
        auto operator-=(StatValue other) -> StatValue&;
        auto operator*=(StatValue other) -> StatValue&;
        auto operator/=(StatValue other) -> StatValue&;

    public:
        static auto Zero() -> StatValue;

    private:
        value_type _value = {};
    };
}
