#include "suspend_for_seconds.h"

#include <pugixml.hpp>
#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/event/timer_event.h"

namespace zerosugar::xr::ai
{
    auto SuspendForSeconds::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        AIController& controller = *blackBoard.Get<AIController*>(AIController::name);

        const int64_t counter = controller.PublishEventCounter();

        Delay(_duration).Then(controller.GetGameInstance().GetStrand(),
            [weak = controller.weak_from_this(), counter]()
            {
                const std::shared_ptr<AIController> controller = weak.lock();
                if (!controller || controller->HasDifferenceEventCounter(counter))
                {
                    return;
                }

                controller->InvokeOnBehaviorTree([](BehaviorTree& bt)
                    {
                        assert(bt.IsWaitFor<event::AwakeBehaviorTree>());

                        bt.Notify(event::AwakeBehaviorTree {});
                    });
            });

        co_await bt::Event<event::AwakeBehaviorTree>();

        co_return true;
    }

    auto SuspendForSeconds::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(SuspendForSeconds& self, const pugi::xml_node& xmlNode)
    {
        if (const auto attr = xmlNode.attribute("time"); attr)
        {
            self._duration = GetMilliFromGameSeconds(attr.as_double());
        }
        else
        {
            assert(false);
        }
    }
}
