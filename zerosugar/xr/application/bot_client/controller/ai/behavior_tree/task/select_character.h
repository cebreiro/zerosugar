#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::bot
{
    class SelectCharacter final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "select_character";

        auto Run() -> bt::node::Result override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(SelectCharacter&, const pugi::xml_node&);
    };
}
