#include "game_entity.h"

namespace zerosugar::xr
{
    auto GameEntity::GetController() const -> IGameController&
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

    void GameEntity::SetController(SharedPtrNotNull<IGameController> controller)
    {
        _controller = std::move(controller);
    }
}
