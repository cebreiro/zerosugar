#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::bt::node
{
    class ForceSuccess final : public Decorator
    {
    public:
        static constexpr const char* name = "force_success";

    public:
        auto Decorate(State state) const->State override;

        friend void from_xml(ForceSuccess&, const pugi::xml_node&);
    };
}
