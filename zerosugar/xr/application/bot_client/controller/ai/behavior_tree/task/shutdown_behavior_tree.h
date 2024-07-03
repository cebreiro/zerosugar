#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::bot
{
    class ShutdownBehaviorTree final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "shutdown_behavior_tree";

        auto Run() -> bt::node::Result override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(ShutdownBehaviorTree&, const pugi::xml_node&);
    };
}
