#pragma once
#include "zerosugar/xr/server/game/instance/entity/component/game_component.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_parallel.h"
#include "zerosugar/xr/server/game/instance/contents/stat/stat_type.h"
#include "zerosugar/xr/server/game/instance/contents/stat/regen_stat.h"
#include "zerosugar/xr/server/game/instance/contents/stat/stable_stat.h"

namespace zerosugar::xr
{
    class StatComponent : public GameComponent
    {
    public:
        StatComponent();

        void AddItemStat(StatType type, StatValue value);
        void SubItemStat(StatType type, StatValue value);

        auto GetHP(std::chrono::system_clock::time_point now = GameExecutionParallel::GetBaseTimePoint()) const -> StatValue;
        auto GetMP(std::chrono::system_clock::time_point now = GameExecutionParallel::GetBaseTimePoint()) const -> StatValue;
        auto GetStamina(std::chrono::system_clock::time_point now = GameExecutionParallel::GetBaseTimePoint()) const -> StatValue;
        auto Get(StatType type) const -> StatValue;

        auto GetMaxHP() const -> StatValue;
        auto GetMaxMP() const -> StatValue;
        auto GetMaxStamina() const -> StatValue;

        void SetHP(StatValue value, std::chrono::system_clock::time_point now = GameExecutionParallel::GetBaseTimePoint());
        void SetMP(StatValue value, std::chrono::system_clock::time_point now = GameExecutionParallel::GetBaseTimePoint());
        void SetStamina(StatValue value, std::chrono::system_clock::time_point now = GameExecutionParallel::GetBaseTimePoint());
        void SetMaxHP(StatValue value);
        void SetMaxMP(StatValue value);
        void SetMaxStamina(StatValue value);

    private:
        auto GetStableStat(StatType type) -> StableStat&;
        auto GetStableStat(StatType type) const -> const StableStat&;

        auto CalculateFinalStat(StatType type, const StableStat& stat) const -> StatValue;

    private:
        RegenStat _hp;
        RegenStat _mp;
        RegenStat _stamina;

        std::array<StableStat, static_cast<int32_t>(StatType::Count)> _stats;
    };
}
