#include "player_local_chat.h"

#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/game_type.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_view.h"

namespace zerosugar::xr::game_task
{
    PlayerLocalChat::PlayerLocalChat(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId, game_time_point_type creationTimePoint)
        : GameTaskBaseParamT(creationTimePoint, std::move(param), NullSelector{})
        , _id(targetId)
    {
    }

    bool PlayerLocalChat::ShouldPrepareBeforeScheduled() const
    {
        return true;
    }

    void PlayerLocalChat::Prepare(GameExecutionSerial& serialContext, bool& quickExit)
    {
        quickExit = true;

        const GamePlayerSnapshot* snapshot = serialContext.GetSnapshotContainer().FindPlayer(_id);
        assert(snapshot);

        std::string chatMessage = std::format("[{}] : {}", snapshot->GetName(), GetParam().message);

        network::game::sc::NotifyChattingMessage packet;
        packet.type = static_cast<int32_t>(ChattingType::Local);
        packet.message = std::move(chatMessage);

        serialContext.GetSnapshotView().Broadcast(packet, *snapshot);
    }

    void PlayerLocalChat::Execute(GameExecutionParallel& parallelContext, NullSelector::target_type)
    {
        (void)parallelContext;

        assert(false);
    }

    void PlayerLocalChat::OnComplete(GameExecutionSerial& serialContext)
    {
        (void)serialContext;

        assert(false);
    }
}
