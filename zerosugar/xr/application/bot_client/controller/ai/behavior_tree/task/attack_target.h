#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::bot
{
    class AttackTarget final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "attack_target";

        auto Run() -> bt::node::Result override;

        auto GetName() const->std::string_view override;

        friend void from_xml(AttackTarget&, const pugi::xml_node&);

    };
}
