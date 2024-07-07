#include "game_execution_parallel.h"

#include "zerosugar/xr/server/game/repository/game_repository_interface.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/game_constants.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr
{
    GameExecutionParallel::GameExecutionParallel(GameInstance& gameInstance)
        : _gameInstance(gameInstance)
    {
    }

    GameExecutionParallel::~GameExecutionParallel()
    {
    }

    void GameExecutionParallel::SummitTask(UniquePtrNotNull<GameTask> task, std::optional<game_controller_id_type> controllerId)
    {
        _gameInstance.Summit(std::move(task), controllerId);
    }

    auto GameExecutionParallel::GetServiceLocator() const -> ServiceLocator&
    {
        return _gameInstance.GetServiceLocator();
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
