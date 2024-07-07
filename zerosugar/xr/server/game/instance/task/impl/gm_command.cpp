#include "gm_command.h"

#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/gm/gm_command_factory_interface.h"
#include "zerosugar/xr/server/game/instance/gm/gm_command_interface.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_view.h"

namespace zerosugar::xr::game_task
{
    GMCommand::GMCommand(GMCommandParam param, game_entity_id_type playerId, game_time_point_type creationTimePoint)
        : GameTaskParamT(creationTimePoint, std::move(param), MainTargetSelector(playerId))
    {
    }

    GMCommand::~GMCommand()
    {
    }

    bool GMCommand::ShouldPrepareBeforeScheduled() const
    {
        return true;
    }

    void GMCommand::Prepare(GameExecutionSerial& serialContext, bool& quickExit)
    {
        quickExit = true;

        const game_entity_id_type playerId = GetSelector<MainTargetSelector>().GetTargetId()[0];
        GamePlayerSnapshot* player = serialContext.GetSnapshotContainer().FindPlayer(playerId);
        assert(player);

        const std::vector<std::string>& args = GetParam().args;
        assert(!args.empty());

        const IGMCommandFactory& factory = serialContext.GetGMCommandFactory();
        const std::shared_ptr<IGMCommand> gmCommand = factory.CreateHandler(args[0]);

        if (!gmCommand)
        {
            serialContext.GetSnapshotView().DisplaySystemMessage(playerId,
                std::format("fail to find gm command[{}]", args[0]));

            return;
        }

        std::span<const std::string> span(args.data() + 1, std::ssize(args) - 1);

        if (!gmCommand->Handle(serialContext, *player, span))
        {
            std::ostringstream oss;
            for (int64_t i = 0; i < std::ssize(span); ++i)
            {
                oss << span[i];
                oss << ' ';
            }

            std::string argsStr = oss.str();
            if (!argsStr.empty())
            {
                argsStr.pop_back();
            }

            serialContext.GetSnapshotView().DisplaySystemMessage(playerId,
                std::format("fail to handle gm command[{}], args: [{}]", args[0], argsStr));
        }
    }

    void GMCommand::Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type)
    {
        (void)parallelContext;
    }

    void GMCommand::OnComplete(GameExecutionSerial& serialContext)
    {
        (void)serialContext;
    }
}
