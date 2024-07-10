#include "gm_log_bt.h"

#include "zerosugar/shared/ai/behavior_tree/log/behavior_tree_log_service_adapter.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/ai_control_service.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"

namespace zerosugar::xr::gm
{
    bool LogBT::HandleCommand(GameExecutionSerial& serialContext, GamePlayerSnapshot& player)
    {
        const GameSnapshotContainer& snapshotContainer = serialContext.GetSnapshotContainer();
        const auto& position = player.GetPosition();

        auto& sector = serialContext.GetSpatialContainer().GetSector(position);

        const Eigen::Vector2d pos2d(position.x(), position.y());

        std::map<double, game_entity_id_type> sorted;

        for (game_entity_id_type monsterId : sector.GetEntities(GameEntityType::Monster))
        {
            const std::optional<Eigen::Vector3d> targetPos = snapshotContainer.FindPosition(monsterId);
            assert(targetPos.has_value());

            const Eigen::Vector2d targetPos2d(targetPos->x(), targetPos->y());

            sorted.emplace((pos2d - targetPos2d).squaredNorm(), monsterId);
        }

        if (sorted.empty())
        {
            return false;
        }

        const GameMonsterSnapshot* monster = snapshotContainer.FindMonster(sorted.begin()->second);
        assert(monster);

        AIControlService& aiControlService = serialContext.GetAIControlService();
        AIController* aiController = aiControlService.FindAIController(monster->GetId());
        assert(aiController);

        aiController->SetBehaviorTreeLogger(std::make_shared<BehaviorTreeLogServiceAdapter>(
            serialContext.GetServiceLocator().Get<ILogService>(), LogLevel::Info
        ));

        return true;
    }
}
