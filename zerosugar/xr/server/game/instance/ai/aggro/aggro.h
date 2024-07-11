#pragma once
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr::ai
{
    class Aggro
    {
    public:
        Aggro(game_entity_id_type id, int64_t value);

        void Add(int64_t value);

        auto GetSourceId() const -> game_entity_id_type;
        auto GetValue() const->int64_t;

        bool operator==(const Aggro& other) const;
        bool operator<(const Aggro& other) const;

    private:
        game_entity_id_type _playerId;
        int64_t _value = 0;
    };
}
