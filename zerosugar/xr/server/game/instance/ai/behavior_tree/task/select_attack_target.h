#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::game
{
    class SelectAttackTarget : public bt::Leaf
    {
    public:
        static constexpr const char* name = "select_attack_target";

        auto Run() -> bt::node::Result override;

        auto GetName() const->std::string_view override;

        friend void from_xml(SelectAttackTarget&, const pugi::xml_node&);
    };
}
