#include "detect_under_attack.h"

#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/aggro/aggro_container.h"

namespace zerosugar::xr::ai
{
    auto DetectUnderAttack::Run() -> bt::node::Result
    {
        AIController& controller = *GetBlackBoard().Get<AIController*>(AIController::name);

        return !controller.GetAggroContainer().Empty();
    }

    auto DetectUnderAttack::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(DetectUnderAttack&, const pugi::xml_node&)
    {
    }
}
