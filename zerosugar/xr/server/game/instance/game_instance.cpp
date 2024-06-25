#include "game_instance.h"

#include "zerosugar/xr/server/game/instance/entity/game_entity_container.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_view_container.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_container.h"

namespace zerosugar::xr
{
    GameInstance::GameInstance(SharedPtrNotNull<execution::IExecutor> executor, service_locator_type serviceLocator,
        game_instance_id_type id, int32_t zoneId)
        : _executor(std::move(executor))
        , _strand(std::make_shared<Strand>(std::move(_executor)))
        , _serviceLocator(std::move(serviceLocator))
        , _id(id)
        , _zoneId(zoneId)
        , _entityContainer(std::make_unique<GameEntityContainer>())
        , _entityViewContainer(std::make_unique<GameEntityViewContainer>())
        //, _spatialContainer(std::make_unique<GameSpatialContainer>(0, 0, 0))
    {
    }

    GameInstance::~GameInstance()
    {
    }

    auto GameInstance::Accept(SharedPtrNotNull<GameClient> client) -> Future<void>
    {
        (void)client;

        co_return;
    }

    auto GameInstance::GetExecutor() const -> execution::IExecutor&
    {
        return *_executor;
    }

    auto GameInstance::GetStrand() const -> Strand&
    {
        return *_strand;
    }

    auto GameInstance::GetServiceLocator() -> service_locator_type&
    {
        return _serviceLocator;
    }

    auto GameInstance::GetId() const -> game_instance_id_type
    {
        return _id;
    }

    auto GameInstance::GetZoneId() const -> int32_t
    {
        return _zoneId;
    }

    auto GameInstance::GetEntityContainer() -> GameEntityContainer&
    {
        return *_entityContainer;
    }

    auto GameInstance::GetEntityContainer() const -> const GameEntityContainer&
    {
        return *_entityContainer;
    }

    auto GameInstance::GetEntityViewContainer() -> GameEntityViewContainer&
    {
        return *_entityViewContainer;
    }

    auto GameInstance::GetEntityViewContainer() const -> const GameEntityViewContainer&
    {
        return *_entityViewContainer;
    }

    auto GameInstance::GetSpatialContainer() -> GameSpatialContainer&
    {
        return *_spatialContainer;
    }

    auto GameInstance::GetSpatialContainer() const -> const GameSpatialContainer&
    {
        return *_spatialContainer;
    }
}
