#pragma once
#include "zerosugar/xr/server/game/instance/task/game_task.h"
#include "zerosugar/xr/server/game/instance/game_type.h"

namespace zerosugar::xr::game_task
{
    class PlayerGlobalChat final : public GameTaskParamT<std::pair<ChattingType, std::string>, NullSelector>
    {
    public:
        explicit PlayerGlobalChat(const std::pair<ChattingType, std::string>& param,
            game_time_point_type creationTimePoint = game_clock_type::now());

    private:
        bool ShouldPrepareBeforeScheduled() const override;
        void Prepare(GameExecutionSerial& serialContext, bool& quickExit) override;

        void Execute(GameExecutionParallel& parallelContext, NullSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;
    };
}
