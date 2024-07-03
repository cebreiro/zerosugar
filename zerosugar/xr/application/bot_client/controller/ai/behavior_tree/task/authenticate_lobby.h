#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::bot
{
    class AuthenticateLobby final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "authenticate_lobby";

        auto Run() -> bt::node::Result override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(AuthenticateLobby&, const pugi::xml_node&);
    };
}
