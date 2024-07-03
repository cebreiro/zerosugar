#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::bot
{
    class Login final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "login";

        auto Run() -> bt::node::Result override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(Login& self, const pugi::xml_node& node);
    };
}
