#include "close_socket.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/event/socket_event.h"

namespace zerosugar::xr::bot
{
    auto CloseSocket::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>(BotController::name);

        controller.Close().Then(controller.GetStrand(), [controller = controller.shared_from_this()]()
            {
                controller->InvokeOnBehaviorTree([](BehaviorTree& behaviorTree)
                    {
                        behaviorTree.NotifyAndResume(event::SocketClosed{});
                    });
            });

        co_await bt::Event<event::SocketClosed>();

        co_return true;
    }

    auto CloseSocket::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(CloseSocket&, const pugi::xml_node&)
    {
    }
}
