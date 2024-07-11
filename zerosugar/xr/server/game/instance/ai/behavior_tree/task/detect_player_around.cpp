#include "detect_player_around.h"

#include <pugixml.hpp>
#include <boost/scope/scope_exit.hpp>
#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/aggro/aggro_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_scanner.h"

namespace zerosugar::xr::ai
{
    auto DetectPlayerAround::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();

        const game_time_point_type now = game_clock_type::now();

        if (game_time_point_type* lastScanTimePoint = blackBoard.GetIf<game_time_point_type>(name))
        {
            if ((now - *lastScanTimePoint) < _interval)
            {
                co_return false;
            }

            *lastScanTimePoint = now;
        }
        else
        {
            blackBoard.Insert(name, now);
        }

        AIController& controller = *blackBoard.Get<AIController*>(AIController::name);

        GameInstance& gameInstance = controller.GetGameInstance();
        const GameSnapshotContainer& snapshotContainer = gameInstance.GetSnapshotContainer();

        const game_entity_id_type entityId = controller.GetEntityId();
        const GameMonsterSnapshot* monster = snapshotContainer.FindMonster(entityId);
        assert(monster);

        int64_t counter = controller.PublishEventCounter();
        struct ScanComplete{};

        GameSpatialScanner& spatialScanner = gameInstance.GetSpatialScanner();

        spatialScanner.Schedule(entityId, _radius, { GameEntityType::Player });
        spatialScanner.SetSignalHandler(entityId, [weak = controller.weak_from_this(), counter]()
            {
                const auto controller = weak.lock();

                if (!controller || controller->HasDifferenceEventCounter(counter))
                {
                    return;
                }

                controller->InvokeOnBehaviorTree([](BehaviorTree& bt)
                    {
                        assert(bt.IsWaitFor<ScanComplete>());
                        bt.Notify(ScanComplete{});
                    });
            });

        co_await bt::Event<ScanComplete>();

        boost::scope::scope_exit exit([entityId, &spatialScanner]()
            {
                spatialScanner.CancelScheduled(entityId);
            });

        std::optional<game_entity_id_type> target = std::nullopt;
        double minDistanceSq = std::numeric_limits<double>::max();

        for (game_entity_id_type playerId : spatialScanner.GetLastScanResult(controller.GetEntityId()))
        {
            const std::optional<Eigen::Vector3d> pos = snapshotContainer.FindPosition(playerId);
            if (!pos.has_value())
            {
                continue;
            }

            const double distanceSq = (*pos - monster->GetPosition()).squaredNorm();
            if (distanceSq < minDistanceSq)
            {
                target = playerId;
            }
        }

        if (target.has_value())
        {
            controller.GetAggroContainer().Add(*target, 1);

            co_return true;
        }

        co_return false;
    }

    auto DetectPlayerAround::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(DetectPlayerAround& self, const pugi::xml_node& xmlNode)
    {
        if (const auto attr = xmlNode.attribute("radius"); attr)
        {
            self._radius = attr.as_double();
        }
        else
        {
            assert(false);
        }

        if (const auto attr = xmlNode.attribute("interval"); attr)
        {
            self._interval = GetMilliFromGameSeconds(attr.as_double());
        }
        else
        {
            assert(false);
        }
    }
}
