#pragma once
#include "zerosugar/xr/server/game/instance/game_type.h"
#include "zerosugar/xr/server/game/repository/game_repository_interface.h"

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
        using service_locator_type = ServiceLocatorT<ILogService, IGameRepository>;

    public:
        GameExecutionParallel() = delete;
        explicit GameExecutionParallel(GameInstance& gameInstance);

        auto GetServiceLocator() const -> service_locator_type&;

        auto GetTaskScheduler() -> GameTaskScheduler&;
        auto GetTaskScheduler() const -> const GameTaskScheduler&;

        auto GetEntityContainer() -> GameEntityContainer&;
        auto GetEntityContainer() const -> const GameEntityContainer&;

    public:
        static auto GetBaseTimePoint() -> game_time_point_type;

    private:
        GameInstance& _gameInstance;
        UniquePtrNotNull<service_locator_type> _serviceLocator;
    };
}
