#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::ai
{
    class BattleTargetWithin final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "battle_target_within";

        auto Run() -> bt::node::Result override;

        auto GetName() const->std::string_view override;

        friend void from_xml(BattleTargetWithin& self, const pugi::xml_node& xmlNode);

    private:
        double _distanceSq = 0.0;
    };
}
