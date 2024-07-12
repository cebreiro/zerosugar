#include "start_move_to_random_position.h"

#include <pugixml.hpp>
#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/movement/bot_movement_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/local_player.h"
#include "zerosugar/xr/data/table/monster.h"
#include "zerosugar/xr/navigation/navigation_service.h"

namespace zerosugar::xr::bot
{
    auto StartMoveToRandomPosition::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>(BotController::name);
        
        NavigationService* navigation = controller.GetNavigation();

        const double radius = SelectRandomRadius(controller.GetRandomEngine());
        LocalPlayer& player = controller.GetLocalPlayer();

        constexpr float speed = 400.f;

        std::optional<Eigen::Vector3d> destPosition = std::nullopt;

        if (navigation)
        {
            struct NaviCompleteRandomPointAroundCircle
            {
                std::optional<navi::FVector> point = std::nullopt;
            };

            const navi::FVector pos(player.GetPosition());

            navigation->GetRandomPointAroundCircle(pos, static_cast<float>(radius))
                .Then(controller.GetStrand(),
                    [weak = controller.weak_from_this()](std::optional<navi::FVector> result)
                    {
                        const auto controller = weak.lock();
                        if (!controller)
                        {
                            return;
                        }

                        controller->InvokeOnBehaviorTree([result](BehaviorTree& bt)
                            {
                                assert(bt.IsWaitFor<NaviCompleteRandomPointAroundCircle>());

                                const NaviCompleteRandomPointAroundCircle event {
                                    .point = result,
                                };

                                bt.Notify(event);
                            });
                    });

            const auto va = co_await bt::Event<NaviCompleteRandomPointAroundCircle>();
            const std::optional<navi::FVector>& point = std::get<NaviCompleteRandomPointAroundCircle>(va).point;

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

            destPosition = player.GetPosition() + rand;
        }

        if (navigation)
        {
            struct NaviCompleteFindStraightPath
            {
                boost::container::static_vector<navi::FVector, navi::constant::max_straight_path_count> points;
            };

            const navi::FVector startPos(player.GetPosition());
            const navi::FVector endPos(*destPosition);

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
                                assert(bt.IsWaitFor<NaviCompleteFindStraightPath>());

                                const NaviCompleteFindStraightPath event {
                                    .points = result,
                                };

                                bt.Notify(event);
                            });
                    });

            const auto va = co_await bt::Event<NaviCompleteFindStraightPath>();
            const auto& points = std::get<NaviCompleteFindStraightPath>(va).points;

            if (!points.empty())
            {
                controller.GetMovementController().MovePath(points, speed);

                co_return true;
            }
        }

        controller.GetMovementController().MoveTo(player.GetPosition(), *destPosition, speed);

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
