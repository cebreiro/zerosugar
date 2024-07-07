#pragma once
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    struct GMCommandParam
    {
        std::vector<std::string> args;
    };

    class GMCommand : public GameTaskParamT<GMCommandParam, MainTargetSelector>
    {
    public:
        GMCommand(GMCommandParam param, game_entity_id_type playerId, game_time_point_type creationTimePoint = game_clock_type::now());
        ~GMCommand();

    private:
        bool ShouldPrepareBeforeScheduled() const override;
        void Prepare(GameExecutionSerial& serialContext, bool& quickExit) override;

        void Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;;
    };
}
