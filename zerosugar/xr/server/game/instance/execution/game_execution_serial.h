#pragma once

namespace zerosugar::xr
{
    class GameInstance;
    class GameEntityViewContainer;
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

        auto GetEntityViewContainer() -> GameEntityViewContainer&;
        auto GetEntityViewContainer() const -> const GameEntityViewContainer&;

        auto GetSpatialContainer() -> GameSpatialContainer&;
        auto GetSpatialContainer() const -> const GameSpatialContainer&;

    private:
        GameInstance& _gameInstance;
        UniquePtrNotNull<service_locator_type> _serviceLocator;
    };
}
