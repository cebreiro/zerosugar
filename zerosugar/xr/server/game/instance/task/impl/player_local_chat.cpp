#include "player_local_chat.h"

#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/view/game_player_view_model.h"
#include "zerosugar/xr/server/game/instance/view/game_view_model_container.h"
#include "zerosugar/xr/server/game/instance/contents/game_constants.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/view/game_view_controller.h"

namespace zerosugar::xr::game_task
{
    PlayerLocalChat::PlayerLocalChat(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId,
        std::chrono::system_clock::time_point creationTimePoint)
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

        const GamePlayerViewModel* viewModel = serialContext.GetViewModelContainer().FindPlayer(_id);
        assert(viewModel);

        std::string chatMessage = std::format("[{}] : {}", viewModel->GetName(), GetParam().message);

        network::game::sc::NotifyChattingMessage packet;
        packet.type = static_cast<int32_t>(game_constans::ChattingType::Local);
        packet.message = std::move(chatMessage);

        serialContext.GetViewController().Broadcast(packet, *viewModel);
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
