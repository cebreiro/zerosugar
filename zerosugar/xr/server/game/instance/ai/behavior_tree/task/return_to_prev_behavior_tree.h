#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::ai
{
    class ReturnToPrevBehaviorTree final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "return_to_prev_behavior_tree";

        auto Run() -> bt::node::Result override;

        auto GetName() const->std::string_view override;

        friend void from_xml(ReturnToPrevBehaviorTree&, const pugi::xml_node&);
    };
}
