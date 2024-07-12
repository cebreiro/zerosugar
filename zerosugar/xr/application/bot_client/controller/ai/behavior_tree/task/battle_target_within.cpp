#include "battle_target_within.h"

#include <pugixml.hpp>
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/local_player.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/monster.h"

namespace zerosugar::xr::bot
{
    auto BattleTargetWithin::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();

        auto battleTarget = blackBoard.GetOr<std::shared_ptr<Monster>>("battle_target", nullptr);
        if (!battleTarget)
        {
            return false;
        }

        BotController& controller = *blackBoard.Get<BotController*>(BotController::name);

        return (controller.GetLocalPlayer().GetPosition() - battleTarget->GetPosition()).squaredNorm() <= _distanceSq;
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
