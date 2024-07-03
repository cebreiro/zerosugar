#include "transition_behavior_tree.h"

#include <pugixml.hpp>
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/event/suspend_event.h"

namespace zerosugar::xr::bot
{
    auto TransitionBehaviorTree::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>("owner");

        controller.Transition(_target);

        // block this bt execution. this coroutine will be destructed on BehaviorTree::Finalize()
        co_await bt::Event<event::SuspendForever>{};

        co_return false;
    }

    auto TransitionBehaviorTree::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(TransitionBehaviorTree& self, const pugi::xml_node& node)
    {
        if (auto attr = node.attribute("target"); attr)
        {
            self._target = attr.as_string();
        }
        else
        {
            assert(false);
        }
    }
}
