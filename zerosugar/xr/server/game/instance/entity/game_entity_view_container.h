#pragma once
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class GameEntityView;

    class GameEntityViewContainer
    {
    public:
        ~GameEntityViewContainer();

        bool Has(game_entity_id_type id) const;


    private:
        
    };
}
