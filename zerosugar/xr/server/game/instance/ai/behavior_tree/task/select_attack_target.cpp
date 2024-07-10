#include "select_attack_target.h"

#include <boost/scope/scope_exit.hpp>
#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_xml.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_scanner.h"

namespace zerosugar::xr::ai
{
    auto SelectAttackTarget::Run() -> bt::node::Result
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

        constexpr double selectDistance = 800.0;

        int64_t counter = controller.PublishEventCounter();
        struct ScanComplete{};

        GameSpatialScanner& spatialScanner = gameInstance.GetSpatialScanner();

        spatialScanner.Schedule(entityId, selectDistance, { GameEntityType::Player });
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

        for (game_entity_id_type playerId : spatialScanner.GetLastScanResult(controller.GetEntityId()))
        {
            const GamePlayerSnapshot* player = snapshotContainer.FindPlayer(playerId);
            if (!player)
            {
                continue;
            }

            blackBoard.InsertOrUpdate<game_entity_id_type>("target", playerId);

            co_return true;
        }

        co_return false;
    }

    auto SelectAttackTarget::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(SelectAttackTarget& self, const pugi::xml_node& xmlNode)
    {
        if (const auto attr = xmlNode.attribute("interval"); attr)
        {
            const auto seconds = std::chrono::duration<double, std::chrono::seconds::period>(attr.as_double());
            self._interval = std::chrono::duration_cast<std::chrono::milliseconds>(seconds);
        }
        else
        {
            assert(false);
        }
    }
}
