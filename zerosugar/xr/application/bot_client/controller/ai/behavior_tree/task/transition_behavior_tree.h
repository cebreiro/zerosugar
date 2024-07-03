#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::bot
{
    class TransitionBehaviorTree final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "transition_behavior_tree";

        auto Run() -> bt::node::Result override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(TransitionBehaviorTree& self, const pugi::xml_node& node);

    private:
        std::string _target;
    };
}
