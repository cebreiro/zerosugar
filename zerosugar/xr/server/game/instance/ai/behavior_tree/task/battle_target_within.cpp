#include "battle_target_within.h"

#include <pugixml.hpp>
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/aggro/aggro_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"

namespace zerosugar::xr::ai
{
    auto BattleTargetWithin::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        AIController& controller = *blackBoard.Get<AIController*>(AIController::name);

        const GamePlayerSnapshot* target = controller.GetAggroContainer().GetPrimaryTarget();
        if (!target)
        {
            return false;
        }

        GameMonsterSnapshot* monster = controller.GetGameInstance().GetSnapshotContainer().FindMonster(controller.GetEntityId());
        assert(monster);

        const Eigen::Vector2d playerPos(target->GetPosition().x(), target->GetPosition().y());
        const Eigen::Vector2d mobPos(monster->GetPosition().x(), monster->GetPosition().y());

        if ((playerPos - mobPos).squaredNorm() > _distanceSq)
        {
            return false;
        }

        return true;
    }

    auto BattleTargetWithin::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(BattleTargetWithin& self, const pugi::xml_node& xmlNode)
    {
        if (auto attr = xmlNode.attribute("distance"); attr)
        {
            const double distance = attr.as_double();
            assert(distance > 0.0);

            self._distanceSq = distance * distance;
        }
        else
        {
            assert(false);
        }
    }
}
