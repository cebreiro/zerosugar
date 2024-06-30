#pragma once

namespace zerosugar::xr
{
    class GameInstance;
    class GameTaskScheduler;
    class GameSnapshotModelContainer;
    class GameSnapshotView;
    class GameSnapshotController;
    class GameSpatialContainer;
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

        auto GetServiceLocator() const -> service_locator_type&;

        auto GetTaskScheduler() -> GameTaskScheduler&;
        auto GetTaskScheduler() const -> const GameTaskScheduler&;

        auto GetSnapshotContainer() -> GameSnapshotModelContainer&;
        auto GetSnapshotContainer() const -> const GameSnapshotModelContainer&;

        auto GetSnapshotController() -> GameSnapshotController&;

        auto GetSnapshotView() -> GameSnapshotView&;

        auto GetSpatialContainer() -> GameSpatialContainer&;
        auto GetSpatialContainer() const -> const GameSpatialContainer&;

    private:
        GameInstance& _gameInstance;
        UniquePtrNotNull<service_locator_type> _serviceLocator;
    };
}
