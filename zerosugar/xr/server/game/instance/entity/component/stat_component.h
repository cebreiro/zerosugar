#pragma once
#include "zerosugar/xr/server/game/instance/entity/component/game_component.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_parallel.h"

namespace zerosugar::xr
{
    enum class StatType
    {
        Attack,
        Defence,
        Str,
        Dex,
        Intell,

        Count,
    };

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

    private:
        value_type _value = {};
    };

    class RegenStat
    {
    public:
        RegenStat() = default;
        RegenStat(StatValue value, StatValue regenValue, StatValue min, StatValue max);

        auto GetValue(std::chrono::system_clock::time_point now) const -> StatValue;
        auto GetMaxValue() const ->StatValue;

        void ChangeRegenValue(StatValue regenValue, std::chrono::system_clock::time_point now);
        void SetValue(StatValue value, std::chrono::system_clock::time_point now);

    private:
        void Update(std::chrono::system_clock::time_point now);

    private:
        StatValue _value;
        StatValue _regenValue;
        StatValue _min;
        StatValue _max;

        // TODO: make and use 'game constants'
        static constexpr auto null_time_point = std::chrono::system_clock::time_point::min();

        mutable std::chrono::system_clock::time_point _lastUpdateTimePoint = null_time_point;
    };

    class FixedStat
    {
    public:
        void AddBaseStat(StatValue value);
        void AddItemStat(StatValue value);

        auto GetBaseStat() const -> StatValue;
        auto GetItemStat() const -> StatValue;

        auto GetFinalValue() const -> StatValue;

    private:
        auto Recalculate() const -> StatValue;

    private:
        StatValue _base;
        StatValue _item;

        mutable bool _dirty = false;
        mutable StatValue _finalValue = {};
    };

    class StatComponent : public GameComponent
    {
    public:
        StatComponent();

        void AddItemStat(StatType type, StatValue value);
        void SubItemStat(StatType type, StatValue value);

        auto GetHP(std::chrono::system_clock::time_point now = GameExecutionParallel::GetBaseTimePoint()) const -> StatValue;
        auto GetMP(std::chrono::system_clock::time_point now = GameExecutionParallel::GetBaseTimePoint()) const-> StatValue;
        auto GetStamina(std::chrono::system_clock::time_point now = GameExecutionParallel::GetBaseTimePoint()) const-> StatValue;
        auto Get(StatType type) const->StatValue;

        auto GetMaxHP() const->StatValue;
        auto GetMaxMP() const->StatValue;
        auto GetMaxStamina() const-> StatValue;

        void SetHP(StatValue value, std::chrono::system_clock::time_point now = GameExecutionParallel::GetBaseTimePoint());
        void SetMP(StatValue value, std::chrono::system_clock::time_point now = GameExecutionParallel::GetBaseTimePoint());

    private:
        auto GetFixedStat(StatType type) -> FixedStat&;
        auto GetFixedStat(StatType type) const -> const FixedStat&;

    private:
        RegenStat _hp;
        RegenStat _mp;
        RegenStat _stamina;

        std::array<FixedStat, static_cast<int32_t>(StatType::Count)> _stats;
    };
}
