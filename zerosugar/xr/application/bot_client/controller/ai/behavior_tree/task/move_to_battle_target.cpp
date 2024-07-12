#include "move_to_battle_target.h"

#include <pugixml.hpp>
#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/movement/bot_movement_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/local_player.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/monster.h"
#include "zerosugar/xr/navigation/navigation_service.h"


namespace zerosugar::xr::bot
{
    auto MoveToBattleTarget::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();

        auto battleTarget = blackBoard.GetOr<std::shared_ptr<Monster>>("battle_target", nullptr);
        if (!battleTarget)
        {
            co_return false;
        }

        BotController& controller = *blackBoard.Get<BotController*>(BotController::name);
        LocalPlayer& player = controller.GetLocalPlayer();

        constexpr float speed = 400.f;

        const auto now = std::chrono::system_clock::now();

        if (std::chrono::system_clock::time_point* lastPathUpdateTime = blackBoard.GetIf<std::chrono::system_clock::time_point>(name))
        {
            if ((now - *lastPathUpdateTime) < _pathUpdateInterval)
            {
                co_return false;
            }

            *lastPathUpdateTime = now;
        }
        else
        {
            blackBoard.Insert(name, now);
        }

        MovementController& movementController = controller.GetMovementController();
        movementController.StopMovement();

        NavigationService* navigation = controller.GetNavigation();

        struct NaviCompletePathFinding
        {
            boost::container::static_vector<navi::FVector, navi::constant::max_straight_path_count> points;
        };

        if (navigation)
        {
            const navi::FVector startPos(player.GetPosition());
            const navi::FVector endPos(battleTarget->GetPosition());

            navigation->FindStraightPath(startPos, endPos)
                .Then(controller.GetStrand(),
                    [weak = controller.weak_from_this()]
                    (const boost::container::static_vector<navi::FVector, navi::constant::max_straight_path_count>& result)
                    {
                        const auto controller = weak.lock();
                        if (!controller)
                        {
                            return;
                        }

                        controller->InvokeOnBehaviorTree([&result](BehaviorTree& bt)
                            {
                                assert(bt.IsWaitFor<NaviCompletePathFinding>());

                                const NaviCompletePathFinding event {
                                    .points = result,
                                };

                                bt.Notify(event);
                            });
                    });

            const auto va = co_await bt::Event<NaviCompletePathFinding>();
            const auto& points = std::get<NaviCompletePathFinding>(va).points;

            if (std::ssize(points) > 1)
            {
                controller.GetMovementController().MovePath(points, speed);

                co_return true;
            }
        }

        controller.GetMovementController().MoveTo(player.GetPosition(), battleTarget->GetPosition(), speed);

        co_return true;
    }

    auto MoveToBattleTarget::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(MoveToBattleTarget& self, const pugi::xml_node& xmlNode)
    {
        if (const auto attr = xmlNode.attribute("path_update"); attr)
        {
            self._pathUpdateInterval = std::chrono::duration_cast<
                std::chrono::milliseconds>(std::chrono::duration<double, std::chrono::seconds::period>(attr.as_double()));
        }
        else
        {
            assert(false);
        }
    }
}
