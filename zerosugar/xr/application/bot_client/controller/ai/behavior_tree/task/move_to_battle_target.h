#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::bot
{
    class MoveToBattleTarget final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "move_to_battle_target";

        auto Run() -> bt::node::Result override;

        auto GetName() const->std::string_view override;

        friend void from_xml(MoveToBattleTarget& self, const pugi::xml_node& xmlNode);


    private:
        std::chrono::milliseconds _pathUpdateInterval;
    };
}
