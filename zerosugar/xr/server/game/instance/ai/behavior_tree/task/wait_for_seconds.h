#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::game
{
    class WaitForSeconds final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "wait_for_seconds";

        auto Run() -> bt::node::Result override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(WaitForSeconds& self, const pugi::xml_node& xmlNode);

    private:
        std::chrono::milliseconds _duration = std::chrono::milliseconds(0);
    };
}
