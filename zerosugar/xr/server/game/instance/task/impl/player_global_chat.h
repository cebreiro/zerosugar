#pragma once
#include "zerosugar/xr/server/game/instance/task/game_task.h"
#include "zerosugar/xr/server/game/instance/contents/game_constants.h"

namespace zerosugar::xr::game_task
{
    class PlayerGlobalChat final : public GameTaskParamT<std::pair<game_constans::ChattingType, std::string>, NullSelector>
    {
    public:
        explicit PlayerGlobalChat(const std::pair<game_constans::ChattingType, std::string>& param,
            std::chrono::system_clock::time_point creationTimePoint = std::chrono::system_clock::now());

    private:
        bool ShouldPrepareBeforeScheduled() const override;
        void Prepare(GameExecutionSerial& serialContext, bool& quickExit) override;

        void Execute(GameExecutionParallel& parallelContext, NullSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;
    };
}
