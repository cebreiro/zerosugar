#include "wait_for_seconds.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_xml.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/event/timer_event.h"

namespace zerosugar::xr::game
{
    auto WaitForSeconds::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        AIController& controller = *blackBoard.Get<AIController*>("controller");

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
                        assert(bt.IsWaitFor<event::AwakebehaviorTree>());

                        bt.Notify(event::AwakebehaviorTree {});
                    });
            });

        co_await bt::Event<event::AwakebehaviorTree>();

        co_return true;
    }

    auto WaitForSeconds::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(WaitForSeconds& self, const pugi::xml_node& xmlNode)
    {
        if (const auto attr = xmlNode.attribute("time"); attr)
        {
            const auto seconds = std::chrono::duration<double, std::chrono::seconds::period>(attr.as_double());
            self._duration = std::chrono::duration_cast<std::chrono::milliseconds>(seconds);
        }
        else
        {
            assert(false);
        }
    }
}
