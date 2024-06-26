#pragma once
#include "zerosugar/xr/server/game/instance/game_instance_id.h"

namespace zerosugar::xr
{
    class GameEntity;
    class GameEntityContainer;
    class GameEntityViewContainer;
    class GameSpatialContainer;
    class GameTaskScheduler;
}

namespace zerosugar::xr
{
    class GameInstance final
        : public std::enable_shared_from_this<GameInstance>
    {
    public:
        using service_locator_type = ServiceLocatorT<ILogService>;

    public:
        GameInstance(SharedPtrNotNull<execution::IExecutor> executor, service_locator_type serviceLocator,
            game_instance_id_type id, int32_t zoneId);
        ~GameInstance();

        auto SpawnEntity(SharedPtrNotNull<GameEntity> entity) -> Future<void>;

        auto GetExecutor() const -> execution::IExecutor&;
        auto GetStrand() const -> Strand&;
        auto GetServiceLocator() -> service_locator_type&;

        auto GetId() const -> game_instance_id_type;
        auto GetZoneId() const -> int32_t;

        auto GetEntityContainer() -> GameEntityContainer&;
        auto GetEntityContainer() const -> const GameEntityContainer&;
        auto GetEntityViewContainer() -> GameEntityViewContainer&;
        auto GetEntityViewContainer() const -> const GameEntityViewContainer&;
        auto GetSpatialContainer() -> GameSpatialContainer&;
        auto GetSpatialContainer() const -> const GameSpatialContainer&;

    private:
        SharedPtrNotNull<execution::IExecutor> _executor;
        SharedPtrNotNull<Strand> _strand;
        service_locator_type _serviceLocator;

        game_instance_id_type _id;
        int32_t _zoneId = 0;

        std::unique_ptr<GameEntityContainer> _entityContainer;
        std::unique_ptr<GameEntityViewContainer> _entityViewContainer;
        std::unique_ptr<GameSpatialContainer> _spatialContainer;
        std::unique_ptr<GameTaskScheduler> _taskScheduler;
    };
}
