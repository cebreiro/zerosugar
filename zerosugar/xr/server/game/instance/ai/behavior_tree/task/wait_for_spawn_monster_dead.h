#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::game
{
    class WaitForSpawnMonsterDead final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "wait_for_spawn_monster_dead";

        auto Run() -> bt::node::Result override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(WaitForSpawnMonsterDead&, const pugi::xml_node&);
    };
}
