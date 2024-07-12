#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::bot
{
    class TimeElapsedAfterMovement final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "time_elapsed_after_movement";

        auto Run() -> bt::node::Result override;

        auto GetName() const->std::string_view override;

        friend void from_xml(TimeElapsedAfterMovement&, const pugi::xml_node&);

    private:
        auto SelectRandomTime(std::mt19937& randomEngine) const -> std::chrono::milliseconds;

    private:
        double _time = 0.0;
        double _rand = 0.0;
    };
}
