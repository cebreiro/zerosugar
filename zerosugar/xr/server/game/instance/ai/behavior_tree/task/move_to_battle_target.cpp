#include "move_to_battle_target.h"

#include <pugixml.hpp>
#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/data/table/monster.h"
#include "zerosugar/xr/navigation/navigation_service.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/game_type.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/aggro/aggro_container.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/event/navigation_event.h"
#include "zerosugar/xr/server/game/instance/ai/movement/movement_controller.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"

namespace zerosugar::xr::ai
{
    auto MoveToBattleTarget::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        AIController& controller = *blackBoard.Get<AIController*>(AIController::name);
        GameInstance& gameInstance = controller.GetGameInstance();
        NavigationService* navigation = gameInstance.GetNavigationService();

        GamePlayerSnapshot* target = controller.GetAggroContainer().GetPrimaryTarget();
        if (!target)
        {
            co_return false;
        }

        const GameMonsterSnapshot* monster = gameInstance.GetSnapshotContainer().FindMonster(controller.GetEntityId());
        assert(monster);

        const float speed = GetSpeedFromClientValue(monster->GetData().GetBase().speed);

        const game_time_point_type now = game_clock_type::now();

        if (game_time_point_type* lastPathUpdateTime = blackBoard.GetIf<game_time_point_type>(name))
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

        if (navigation)
        {
            const int64_t eventCounter = controller.PublishEventCounter();
            const navi::FVector startPos(monster->GetPosition());
            const navi::FVector endPos(target->GetPosition());

            navigation->FindStraightPath(startPos, endPos)
                .Then(gameInstance.GetStrand(),
                    [weak = controller.weak_from_this(), eventCounter]
                    (const boost::container::static_vector<navi::FVector, navi::constant::max_straight_path_count>& result)
                    {
                        const auto controller = weak.lock();
                        if (!controller || controller->HasDifferenceEventCounter(eventCounter))
                        {
                            return;
                        }

                        controller->InvokeOnBehaviorTree([&result](BehaviorTree& bt)
                            {
                                assert(bt.IsWaitFor<event::NaviCompleteFindStraightPath>());

                                const event::NaviCompleteFindStraightPath event {
                                    .points = result,
                                };

                                bt.Notify(event);
                            });
                    });

            const auto va = co_await bt::Event<event::NaviCompleteFindStraightPath>();
            const auto& points = std::get<event::NaviCompleteFindStraightPath>(va).points;

            if (!points.empty())
            {
                controller.GetMovementController().MovePath(points, speed);

                co_return true;
            }
        }

        controller.GetMovementController().MoveTo(monster->GetPosition(), target->GetPosition(), speed);

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
            self._pathUpdateInterval = GetMilliFromGameSeconds(attr.as_double());
        }
        else
        {
            assert(false);
        }
    }
}
