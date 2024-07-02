#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace pugi
{
    class xml_node;
}

namespace zerosugar::bt::node
{
    class Selector final : public Branch
    {
    public:
        static constexpr const char* name = "selector";

    public:
        bool ShouldContinue(State childState) const override;

        friend void from_xml(Selector&, const pugi::xml_node&);
    };
}
