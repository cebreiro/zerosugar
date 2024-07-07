#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"
#include "zerosugar/xr/server/game/instance/game_type.h"

namespace zerosugar::xr::game
{
    class SelectAttackTarget final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "select_attack_target";

        auto Run() -> bt::node::Result override;

        auto GetName() const->std::string_view override;

        friend void from_xml(SelectAttackTarget& self, const pugi::xml_node& xmlNode);

    private:
        std::chrono::milliseconds _interval = std::chrono::seconds(3);

        game_time_point_type _lastScanTimePoint = game_clock_type::now();
    };
}
