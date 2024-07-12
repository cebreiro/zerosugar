#include "shutdown_behavior_tree.h"

#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"

namespace zerosugar::xr::bot
{
    auto ShutdownBehaviorTree::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>(BotController::name);

        controller.Shutdown(std::format("called on {}", name));


        struct SuspendForever{};

        co_await bt::Event<SuspendForever>();

        co_return false;
    }

    auto ShutdownBehaviorTree::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(ShutdownBehaviorTree&, const pugi::xml_node&)
    {
    }
}
