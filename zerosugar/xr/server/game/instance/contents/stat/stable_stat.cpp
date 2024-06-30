#include "stable_stat.h"

namespace zerosugar::xr
{
    void StableStat::AddBaseStat(StatValue value)
    {
        _base += value;

        _finalValueInvalidated = true;
    }

    void StableStat::AddItemStat(StatValue value)
    {
        _item += value;

        _finalValueInvalidated = true;
    }

    auto StableStat::GetBaseStat() const -> StatValue
    {
        return _base;
    }

    auto StableStat::GetItemStat() const -> StatValue
    {
        return _item;
    }

    bool StableStat::IsCalculationNeeded() const
    {
        return _finalValueInvalidated;
    }

    auto StableStat::GetSum() const -> StatValue
    {
        return _base + _item;
    }

    auto StableStat::GetFinalValue() const -> StatValue
    {
        assert(!_finalValueInvalidated);

        return _finalValue;
    }

    void StableStat::SetFinalStat(StatValue stat)
    {
        _finalValueInvalidated = false;

        _finalValue = stat;
    }
}
