#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::ai
{
    class SuspendForSeconds final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "suspend_for_seconds";

        auto Run() -> bt::node::Result override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(SuspendForSeconds& self, const pugi::xml_node& xmlNode);

    private:
        std::chrono::milliseconds _duration = std::chrono::milliseconds(0);
    };
}
