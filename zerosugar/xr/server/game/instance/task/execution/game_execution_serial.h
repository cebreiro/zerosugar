#pragma once
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class GameInstance;
    class GameTaskScheduler;
    class GameSnapshotContainer;
    class GameSnapshotView;
    class GameSnapshotController;
    class GameSpatialContainer;

    class AIControlService;
}

namespace zerosugar::xr::data
{
    struct Map;
}

namespace zerosugar::xr
{
    class GameExecutionSerial
    {
    public:
        using service_locator_type = ServiceLocatorT<ILogService>;

    public:
        GameExecutionSerial() = delete;

        explicit GameExecutionSerial(GameInstance& gameInstance);

        auto PublishEntityId(GameEntityType type) -> game_entity_id_type;

        auto GetServiceLocator() const -> service_locator_type&;
        auto GetMapData() const -> const data::Map&;

        auto GetTaskScheduler() -> GameTaskScheduler&;
        auto GetTaskScheduler() const -> const GameTaskScheduler&;

        auto GetSnapshotContainer() -> GameSnapshotContainer&;
        auto GetSnapshotContainer() const -> const GameSnapshotContainer&;

        auto GetSnapshotController() -> GameSnapshotController&;

        auto GetSnapshotView() -> GameSnapshotView&;

        auto GetSpatialContainer() -> GameSpatialContainer&;
        auto GetSpatialContainer() const -> const GameSpatialContainer&;

        auto GetAIControlService() -> AIControlService&;

    private:
        GameInstance& _gameInstance;
        UniquePtrNotNull<service_locator_type> _serviceLocator;
    };
}
