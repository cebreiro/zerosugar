#include "return_to_prev_behavior_tree.h"

#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"

namespace zerosugar::xr::ai
{
    auto ReturnToPrevBehaviorTree::Run() -> bt::node::Result
    {
        AIController& controller = *GetBlackBoard().Get<AIController*>(AIController::name);

        struct ReturnBehaviorTreeFail{};

        const int64_t eventCount = controller.PublishEventCounter();

        controller.ReturnPrevBehaviorTree()
            .Then(controller.GetGameInstance().GetStrand(),
                [weak = controller.weak_from_this(), eventCount]([[maybe_unused]] bool result)
                {
                    assert(result);

                    const auto controller = weak.lock();
                    if (!controller || controller->HasDifferenceEventCounter(eventCount))
                    {
                        return;
                    }

                    controller->InvokeOnBehaviorTree([](BehaviorTree& bt)
                        {
                            assert(bt.IsWaitFor<ReturnBehaviorTreeFail>());

                            bt.Notify(ReturnBehaviorTreeFail{});
                        });

                });

        co_await bt::Event<ReturnBehaviorTreeFail>();

        co_return false;
    }

    auto ReturnToPrevBehaviorTree::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(ReturnToPrevBehaviorTree&, const pugi::xml_node&)
    {
    }
}
