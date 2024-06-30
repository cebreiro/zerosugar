#include "player_global_chat.h"

#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_view.h"

namespace zerosugar::xr::game_task
{
    PlayerGlobalChat::PlayerGlobalChat(const std::pair<ChattingType, std::string>& param, game_time_point_type creationTimePoint)
        : GameTaskParamT(creationTimePoint, param, NullSelector{})
    {
    }

    bool PlayerGlobalChat::ShouldPrepareBeforeScheduled() const
    {
        return true;
    }

    void PlayerGlobalChat::Prepare(GameExecutionSerial& serialContext, bool& quickExit)
    {
        quickExit = true;

        network::game::sc::NotifyChattingMessage packet;
        packet.type = static_cast<int32_t>(GetParam().first);
        packet.message = GetParam().second;

        serialContext.GetSnapshotView().Broadcast(packet);
    }

    void PlayerGlobalChat::Execute(GameExecutionParallel& parallelContext, NullSelector::target_type)
    {
        (void)parallelContext;

        assert(false);
    }

    void PlayerGlobalChat::OnComplete(GameExecutionSerial& serialContext)
    {
        (void)serialContext;

        assert(false);
    }
}
