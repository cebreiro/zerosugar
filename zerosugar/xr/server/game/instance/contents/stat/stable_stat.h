#pragma once
#include "zerosugar/xr/server/game/instance/contents/stat/stat_value.h"

namespace zerosugar::xr
{
    class StableStat
    {
    public:
        void AddBaseStat(StatValue value);
        void AddItemStat(StatValue value);

        auto GetBaseStat() const -> StatValue;
        auto GetItemStat() const -> StatValue;

        bool IsCalculationNeeded() const;

        auto GetSum() const -> StatValue;
        auto GetFinalValue() const -> StatValue;

        void SetFinalStat(StatValue stat);

    private:
        StatValue _base;
        StatValue _item;

        StatValue _finalValue;

        bool _finalValueInvalidated = false;
    };
}
