#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class GameEntityView;
}

namespace zerosugar::xr
{
    class GameEntityViewContainer
    {
    public:
        ~GameEntityViewContainer();

        bool Has(game_entity_id_type id) const;

        bool Add(UniquePtrNotNull<GameEntityView> entityView);
        bool Remove(game_entity_id_type id);

        auto Find(game_entity_id_type id) -> GameEntityView*;
        auto Find(game_entity_id_type id) const -> const GameEntityView*;

    private:
        boost::unordered::unordered_flat_map<game_entity_id_type, UniquePtrNotNull<GameEntityView>> _entityViews;
    };
}
