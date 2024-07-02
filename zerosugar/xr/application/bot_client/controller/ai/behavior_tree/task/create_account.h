#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::bot
{
    class CreateAccount final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "create_account";

        auto Run() -> bt::node::Result override;

        friend void from_xml(CreateAccount& self, const pugi::xml_node& node);
    };
}
