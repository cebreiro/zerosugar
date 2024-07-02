#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::bot
{
    class IsConnectedToLogin final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "is_connected_to_login";

        auto Run() -> bt::node::Result final;

        friend void from_xml(IsConnectedToLogin&, const pugi::xml_node&);
    };

}
