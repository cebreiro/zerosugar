#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::bot
{
    class DungeonMatch final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "dungeon_match";

        auto Run() -> bt::node::Result override;

        auto GetName() const->std::string_view override;

        friend void from_xml(DungeonMatch& self, const pugi::xml_node& xmlNode);

    private:
        int32_t _mapId = 0;
    };
}
