#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::bot
{
    class SuspendForever : public bt::Leaf
    {
    public:
        static constexpr const char* name = "suspend_forever";

        auto Run() -> bt::node::Result override;

        auto GetName() const->std::string_view override;

        friend void from_xml(SuspendForever&, const pugi::xml_node&);
    };
}
