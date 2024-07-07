#include "select_attack_target.h"

#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_xml.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/grid/game_spatial_container.h"

namespace zerosugar::xr::game
{
    auto SelectAttackTarget::Run() -> bt::node::Result
    {
        const game_time_point_type now = game_clock_type::now();
        if ((now - _lastScanTimePoint) < _interval)
        {
            co_return false;
        }

        _lastScanTimePoint = now;

        bt::BlackBoard& blackBoard = GetBlackBoard();
        AIController& controller = *blackBoard.Get<AIController*>("controller");

        const GameInstance& gameInstance = controller.GetGameInstance();
        const GameSnapshotContainer& snapshotContainer = gameInstance.GetSnapshotContainer();
        const GameSpatialContainer& spatialContainer = gameInstance.GetSpatialContainer();

        const GameMonsterSnapshot* monster = snapshotContainer.FindMonster(controller.GetEntityId());
        assert(monster);

        constexpr double selectDistance = 800.0;
        constexpr double selectDistanceSq = selectDistance * selectDistance;

        const auto& position = monster->GetPosition();
        const double offsetX = position.x() + spatialContainer.GetPositionOffset();
        const double offsetY = position.y() + spatialContainer.GetPositionOffset();

        const GameSpatialSector& sector = spatialContainer.GetSector(position.x(), position.y());

        const GameSpatialCell& center = sector.GetCenter();
        const auto filter = [&offsetX, &offsetY, &center, &selectDistanceSq](const GameSpatialCell& cell)
            {
                if (!cell.HasEntity(GameEntityType::Player))
                {
                    return false;
                }

                double diffX = 0.0;
                double diffY = 0.0;

                if (center.GetId().GetX() != cell.GetId().GetX())
                {
                    diffX = std::abs(offsetX - cell.GetLeftX());
                }

                if (center.GetId().GetY() != cell.GetId().GetY())
                {
                    diffY = std::abs(offsetY - cell.GetTopY());
                }

                const double diffSq = (diffX * diffX) + (diffY * diffY);
                if (diffSq > selectDistanceSq)
                {
                    return false;
                }

                return true;
            };

        auto range = sector.GetCells()
            | std::views::filter(filter)
            | std::views::transform([](const GameSpatialCell& cell) -> std::span<const game_entity_id_type>
                {
                    return cell.GetEntities(GameEntityType::Player);
                })
            | std::views::join;

        for (const game_entity_id_type playerId : range)
        {
            const GamePlayerSnapshot* player = snapshotContainer.FindPlayer(playerId);
            assert(player);

            if ((position - player->GetPosition()).squaredNorm() <= selectDistanceSq)
            {
                blackBoard.InsertOrUpdate<game_entity_id_type>("target", playerId);

                co_return true;
            }
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
