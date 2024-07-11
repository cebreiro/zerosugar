#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"
#include "zerosugar/xr/server/game/instance/game_type.h"

namespace zerosugar::xr::ai
{
    class DetectPlayerAround final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "detect_player_around";

        auto Run() -> bt::node::Result override;

        auto GetName() const->std::string_view override;

        friend void from_xml(DetectPlayerAround& self, const pugi::xml_node& xmlNode);

    private:
        double _radius = 0.0;
        std::chrono::milliseconds _interval = std::chrono::seconds(3);
    };
}
