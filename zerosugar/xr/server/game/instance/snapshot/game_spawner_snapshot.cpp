#include "game_spawner_snapshot.h"

#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"

namespace zerosugar::xr
{
    GameSpawnerSnapshot::GameSpawnerSnapshot(IGameController& controller, game_entity_id_type entityId)
        : _controller(controller)
        , _id(entityId)
    {
    }

    GameSpawnerSnapshot::~GameSpawnerSnapshot()
    {
    }

    void GameSpawnerSnapshot::Initialize(const GameEntity& entity)
    {
        const MovementComponent& movementComponent = entity.GetComponent<MovementComponent>();
        
        _position = movementComponent.GetPosition();
    }

    auto GameSpawnerSnapshot::GetController() -> IGameController&
    {
        return _controller;
    }

    auto GameSpawnerSnapshot::GetId() const -> game_entity_id_type
    {
        return _id;
    }

    auto GameSpawnerSnapshot::GetPosition() const -> const Eigen::Vector3d&
    {
        return _position;
    }
}
