#include "game_entity_view.h"

#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/component/movement_component.h"

namespace zerosugar::xr
{
    GameEntityView::GameEntityView(const GameEntity& entity)
        : _id(entity.GetId())
    {
        const MovementComponent& movementComponent = entity.GetComponent<MovementComponent>();
        SetPosition(movementComponent.GetPosition());
    }

    auto GameEntityView::GetId() const -> game_entity_id_type
    {
        return _id;
    }

    auto GameEntityView::GetPosition() const -> const Eigen::Vector3d&
    {
        return _position;
    }

    void GameEntityView::SetPosition(const Eigen::Vector3d& position)
    {
        _position = position;
    }
}
