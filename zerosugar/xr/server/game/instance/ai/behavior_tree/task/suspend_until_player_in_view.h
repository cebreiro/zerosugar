#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::ai
{
    class SuspendUntilPlayerInView final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "suspend_until_player_in_view";

        auto Run() -> bt::node::Result override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(SuspendUntilPlayerInView&, const pugi::xml_node&);
    };
}
