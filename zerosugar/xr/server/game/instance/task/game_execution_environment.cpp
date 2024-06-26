#include "game_execution_environment.h"

#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr
{
    auto GameExecutionEnvironment::GetCurrentTimePoint() -> std::chrono::system_clock::time_point
    {
        if (GameTask::IsInExecution())
        {
            return GameTask::GetLocalInstance().GetBaseTime();
        }

        return std::chrono::system_clock::time_point::min();
    }
}
