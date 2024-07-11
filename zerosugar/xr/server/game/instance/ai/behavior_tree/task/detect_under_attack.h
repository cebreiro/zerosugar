#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::ai
{
    class DetectUnderAttack final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "detect_under_attack";

        auto Run() -> bt::node::Result override;

        auto GetName() const->std::string_view override;

        friend void from_xml(DetectUnderAttack&, const pugi::xml_node&);
    };
}
