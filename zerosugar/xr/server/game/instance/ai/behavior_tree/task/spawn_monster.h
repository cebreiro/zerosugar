#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::game
{
    class SpawnMonster : public bt::Leaf
    {
    public:
        static constexpr const char* name = "spawn_monster";

        auto Run() -> bt::node::Result override;

        auto GetName() const->std::string_view override;

        friend void from_xml(SpawnMonster&, const pugi::xml_node&);
    };
}
