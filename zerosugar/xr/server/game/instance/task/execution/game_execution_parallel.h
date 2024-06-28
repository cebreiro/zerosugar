#pragma once

namespace zerosugar::xr
{
    class GameInstance;
    class GameTaskScheduler;
    class GameEntityContainer;
}

namespace zerosugar::xr
{
    class GameExecutionParallel
    {
    public:
        using service_locator_type = ServiceLocatorT<ILogService>;

    public:
        GameExecutionParallel() = delete;
        explicit GameExecutionParallel(GameInstance& gameInstance);

        auto GetServiceLocator() const -> service_locator_type&;

        auto GetTaskScheduler() -> GameTaskScheduler&;
        auto GetTaskScheduler() const -> const GameTaskScheduler&;

        auto GetEntityContainer() -> GameEntityContainer&;
        auto GetEntityContainer() const -> const GameEntityContainer&;

    public:
        static auto GetBaseTimePoint() -> std::chrono::system_clock::time_point;

    private:
        GameInstance& _gameInstance;
        UniquePtrNotNull<service_locator_type> _serviceLocator;
    };
}
