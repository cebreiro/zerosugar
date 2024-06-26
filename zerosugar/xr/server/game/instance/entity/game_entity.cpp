#include "game_entity.h"

namespace zerosugar::xr
{
    auto GameEntity::GetController() const -> IGameEntityController&
    {
        assert(_controller);

        return *_controller;
    }

    auto GameEntity::GetId() const -> game_entity_id_type
    {
        return _id;
    }

    void GameEntity::SetId(game_entity_id_type id)
    {
        _id = id;
    }

    void GameEntity::SetController(SharedPtrNotNull<IGameEntityController> controller)
    {
        _controller = std::move(controller);
    }
}
