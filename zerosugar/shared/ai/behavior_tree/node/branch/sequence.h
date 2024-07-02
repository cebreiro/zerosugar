#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace pugi
{
    class xml_node;
}

namespace zerosugar::bt::node
{
    class Sequence : public Branch
    {
    public:
        static constexpr const char* name = "sequence";

    public:
        bool ShouldContinue(node::State childState) const override;

        friend void from_xml(Sequence&, const pugi::xml_node&);
    };
}
