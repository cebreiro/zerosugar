#include "stat_component.h"

namespace zerosugar::xr
{
    StatComponent::StatComponent()
    {
    }

    void StatComponent::AddItemStat(StatType type, StatValue value)
    {
        GetStableStat(type).AddItemStat(value);
    }

    void StatComponent::SubItemStat(StatType type, StatValue value)
    {
        GetStableStat(type).AddItemStat(-value);
    }

    auto StatComponent::GetHP(game_time_point_type now) const -> StatValue
    {
        return _hp.GetValue(now);
    }

    auto StatComponent::GetMP(game_time_point_type now) const -> StatValue
    {
        return _mp.GetValue(now);
    }

    auto StatComponent::GetStamina(game_time_point_type now) const -> StatValue
    {
        return _stamina.GetValue(now);
    }

    auto StatComponent::Get(StatType type) const -> StatValue
    {
        const StableStat& stableStat = GetStableStat(type);

        if (stableStat.IsCalculationNeeded())
        {
            const StatValue finalStat = CalculateFinalStat(type, stableStat);

            const_cast<StableStat&>(stableStat).SetFinalStat(finalStat);
        }

        return stableStat.GetFinalValue();
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

    void StatComponent::SetHP(StatValue value, game_time_point_type now)
    {
        _hp.SetValue(value, now);
    }

    void StatComponent::SetMP(StatValue value, game_time_point_type now)
    {
        _mp.SetValue(value, now);
    }

    void StatComponent::SetStamina(StatValue value, game_time_point_type now)
    {
        _stamina.SetValue(value, now);
    }

    void StatComponent::SetHPRegen(bool value, game_time_point_type now)
    {
        _hp.SetRegen(value, now);
    }

    void StatComponent::SetMaxHP(StatValue value)
    {
        _hp.SetMaxValue(value);
    }

    void StatComponent::SetMaxMP(StatValue value)
    {
        _mp.SetMaxValue(value);
    }

    void StatComponent::SetMaxStamina(StatValue value)
    {
        _stamina.SetMaxValue(value);
    }

    auto StatComponent::GetStableStat(StatType type) -> StableStat&
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_stats));

        return _stats[index];
    }

    auto StatComponent::GetStableStat(StatType type) const -> const StableStat&
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_stats));

        return _stats[index];
    }

    auto StatComponent::CalculateFinalStat(StatType type, const StableStat& stat) const -> StatValue
    {
        switch (type)
        {
        case StatType::Attack:
        {
            return stat.GetSum() + Get(StatType::Str) * StatValue(5.0);
        }
        case StatType::Defence:
        {
            return stat.GetSum() + Get(StatType::Dex) * StatValue(5.0);
        }
        break;
        case StatType::Str:
        case StatType::Dex:
        case StatType::Intell:
            return stat.GetSum();
        case StatType::Count:
        default:
            assert(false);
        }

        return StatValue::Zero();
    }
}
