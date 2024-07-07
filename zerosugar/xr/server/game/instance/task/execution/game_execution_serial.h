#pragma once
#include "zerosugar/xr/server/game/instance/controller/game_controller_id.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class GameInstance;
    class GameTask;
    class GameTaskScheduler;
    class GameSnapshotContainer;
    class GameSnapshotView;
    class GameSnapshotController;
    class GameSpatialContainer;

    class AIControlService;
    class NavigationService;

    class IGMCommandFactory;
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
        GameExecutionSerial() = delete;

        explicit GameExecutionSerial(GameInstance& gameInstance);
        ~GameExecutionSerial();

        auto Hold() -> std::shared_ptr<GameExecutionSerial>;

        void SummitTask(UniquePtrNotNull<GameTask> task, std::optional<game_controller_id_type> controllerId = std::nullopt);

        auto PublishEntityId(GameEntityType type) -> game_entity_id_type;

        auto GetServiceLocator() const -> ServiceLocator&;
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
        auto GetNavigationService() -> NavigationService*;

        auto GetGMCommandFactory() const -> const IGMCommandFactory&;

    private:
        GameInstance& _gameInstance;
    };
}
