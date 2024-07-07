#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::bt::node
{
    class SuspendForever final : public Leaf
    {
    public:
        static constexpr const char* name = "suspend_forever";

        auto Run() -> Result override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(SuspendForever&, const pugi::xml_node&);
    };
}
