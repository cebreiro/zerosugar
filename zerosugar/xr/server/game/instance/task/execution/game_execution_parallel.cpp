#include "game_execution_parallel.h"

#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/game_constants.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr
{
    GameExecutionParallel::GameExecutionParallel(GameInstance& gameInstance)
        : _gameInstance(gameInstance)
        , _serviceLocator(std::make_unique<service_locator_type>(gameInstance.GetServiceLocator()))
    {
    }

    auto GameExecutionParallel::GetServiceLocator() const -> service_locator_type&
    {
        return *_serviceLocator;
    }

    auto GameExecutionParallel::GetTaskScheduler() -> GameTaskScheduler&
    {
        return _gameInstance.GetTaskScheduler();
    }

    auto GameExecutionParallel::GetTaskScheduler() const -> const GameTaskScheduler&
    {
        return _gameInstance.GetTaskScheduler();
    }

    auto GameExecutionParallel::GetEntityContainer() -> GameEntityContainer&
    {
        return _gameInstance.GetEntityContainer();
    }

    auto GameExecutionParallel::GetEntityContainer() const -> const GameEntityContainer&
    {
        return _gameInstance.GetEntityContainer();
    }

    auto GameExecutionParallel::GetBaseTimePoint() -> game_time_point_type
    {
        if (GameTask::IsInExecution())
        {
            return GameTask::GetLocalInstance().GetBaseTime();
        }

        return game_constant::null_time_point;
    }
}
