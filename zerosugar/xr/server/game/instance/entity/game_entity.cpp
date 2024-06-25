#include "game_entity.h"

namespace zerosugar::xr
{
    GameEntity::GameEntity(game_entity_id_type id)
        : _id(id)
    {
    }

    auto GameEntity::GetId() const -> game_entity_id_type
    {
        return _id;
    }
}
