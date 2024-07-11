#include "update_battle_target.h"

#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/aggro/aggro_container.h"

namespace zerosugar::xr::ai
{
    auto UpdateBattleTarget::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        AIController& controller = *blackBoard.Get<AIController*>(AIController::name);

        AggroContainer& aggroContainer = controller.GetAggroContainer();

        aggroContainer.Update();

        return true;
    }

    auto UpdateBattleTarget::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(UpdateBattleTarget&, const pugi::xml_node&)
    {
    }
}
