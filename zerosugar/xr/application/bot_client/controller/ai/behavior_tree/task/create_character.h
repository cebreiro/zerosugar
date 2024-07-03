#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::bot
{
    class CreateCharacter final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "create_character";

        auto Run() -> bt::node::Result override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(CreateCharacter&, const pugi::xml_node&);
    };
}
