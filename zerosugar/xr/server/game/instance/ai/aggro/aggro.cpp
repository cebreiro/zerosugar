#include "aggro.h"

namespace zerosugar::xr::ai
{
    Aggro::Aggro(game_entity_id_type id, int64_t value)
        : _playerId(id)
        , _value(value)
    {
    }

    void Aggro::Add(int64_t value)
    {
        _value += value;
    }

    auto Aggro::GetSourceId() const -> game_entity_id_type
    {
        return _playerId;
    }

    auto Aggro::GetValue() const -> int64_t
    {
        return _value;
    }

    bool Aggro::operator==(const Aggro& other) const
    {
        return _playerId == other._playerId;
    }

    bool Aggro::operator<(const Aggro& other) const
    {
        return _value < other._value;
    }
}
