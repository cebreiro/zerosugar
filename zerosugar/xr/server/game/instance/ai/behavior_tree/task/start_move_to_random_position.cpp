#include "start_move_to_random_position.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_xml.h"
#include "zerosugar/xr/navigation/navigation_service.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/event/navigation_event.h"
#include "zerosugar/xr/server/game/instance/ai/movement/movement_controller.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"

namespace zerosugar::xr::ai
{
    auto StartMoveToRandomPosition::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        AIController& controller = *blackBoard.Get<AIController*>(AIController::name);
        GameInstance& gameInstance = controller.GetGameInstance();
        NavigationService* navigation = gameInstance.GetNavigationService();

        const double radius = SelectRandomRadius(controller.GetRandomEngine());
        const GameMonsterSnapshot* monster = gameInstance.GetSnapshotContainer().FindMonster(controller.GetEntityId());
        assert(monster);

        std::optional<Eigen::Vector3d> destPosition = std::nullopt;

        if (navigation)
        {
            const int64_t eventCounter = controller.PublishEventCounter();
            const navi::FVector pos(monster->GetPosition());

            navigation->GetRandomPointAroundCircle(pos, static_cast<float>(radius))
                .Then(gameInstance.GetStrand(),
                    [weak = controller.weak_from_this(), eventCounter](std::optional<navi::FVector> result)
                    {
                        const auto controller = weak.lock();
                        if (!controller || controller->HasDifferenceEventCounter(eventCounter))
                        {
                            return;
                        }

                        controller->InvokeOnBehaviorTree([result](BehaviorTree& bt)
                            {
                                assert(bt.IsWaitFor<event::NaviCompleteRandomPointAroundCircle>());

                                const event::NaviCompleteRandomPointAroundCircle event {
                                    .point = result,
                                };

                                bt.Notify(event);
                            });
                    });

            const auto va = co_await bt::Event<event::NaviCompleteRandomPointAroundCircle>();
            const std::optional<navi::FVector>& point = std::get<event::NaviCompleteRandomPointAroundCircle>(va).point;

            if (point.has_value())
            {
                destPosition = Eigen::Vector3d(point->GetX(), point->GetY(), point->GetZ());
            }
        }

        if (!destPosition.has_value())
        {
            std::uniform_real_distribution<double> dist(0.0, 360.0);
            const Eigen::AngleAxisd axis(dist(controller.GetRandomEngine()) * std::numbers::pi / 180.0, Eigen::Vector3d::UnitZ());

            const Eigen::Vector3d rand = axis.toRotationMatrix() * Eigen::Vector3d(radius, 0, 0);

            destPosition = monster->GetPosition() + rand;
        }

        if (navigation)
        {
            const int64_t eventCounter = controller.PublishEventCounter();
            const navi::FVector startPos(monster->GetPosition());
            const navi::FVector endPos(*destPosition);

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
                controller.GetMovementController().MovePath(points, 500.f);

                co_return true;
            }
        }

        controller.GetMovementController().MoveTo(monster->GetPosition(), *destPosition, 500.f);

        co_return true;
    }

    auto StartMoveToRandomPosition::GetName() const -> std::string_view
    {
        return name;
    }

    auto StartMoveToRandomPosition::SelectRandomRadius(std::mt19937& randomEngine) const -> double
    {
        std::uniform_real_distribution<double> dist(-_rand, _rand);

        return std::max(300.0, _radius + dist(randomEngine));
    }

    void from_xml(StartMoveToRandomPosition& self, const pugi::xml_node& xmlNode)
    {
        if (const auto attr = xmlNode.attribute("radius"); attr)
        {
            self._radius = attr.as_double();
        }

        if (const auto attr = xmlNode.attribute("rand"); attr)
        {
            self._radius = attr.as_double();
        }
    }
}
