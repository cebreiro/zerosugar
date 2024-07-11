#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::ai
{
    class UpdateBattleTarget final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "update_battle_target";

        auto Run() -> bt::node::Result override;

        auto GetName() const->std::string_view override;

        friend void from_xml(UpdateBattleTarget&, const pugi::xml_node&);
    };
}
