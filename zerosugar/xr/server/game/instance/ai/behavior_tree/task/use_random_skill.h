#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::ai
{
    class UseRandomSkill final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "use_random_skill";

        auto Run() -> bt::node::Result override;

        auto GetName() const->std::string_view override;

        friend void from_xml(UseRandomSkill&, const pugi::xml_node&);
    };
}
