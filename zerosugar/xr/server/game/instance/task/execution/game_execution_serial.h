#pragma once

namespace zerosugar::xr
{
    class GameInstance;
    class GameTaskScheduler;
    class GameViewController;
    class GameViewModelContainer;
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

        auto GetViewController() -> GameViewController&;

        auto GetViewModelContainer() -> GameViewModelContainer&;
        auto GetViewModelContainer() const -> const GameViewModelContainer&;

        auto GetSpatialContainer() -> GameSpatialContainer&;
        auto GetSpatialContainer() const -> const GameSpatialContainer&;

    private:
        GameInstance& _gameInstance;
        UniquePtrNotNull<service_locator_type> _serviceLocator;
    };
}
