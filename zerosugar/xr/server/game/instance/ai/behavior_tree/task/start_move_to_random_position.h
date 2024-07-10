#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::ai
{
    class StartMoveToRandomPosition final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "start_move_to_random_position";

        auto Run() -> bt::node::Result override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(StartMoveToRandomPosition&, const pugi::xml_node&);

    private:
        auto SelectRandomRadius(std::mt19937& randomEngine) const -> double;

    private:
        double _radius = 0.0;
        double _rand = 0.0;
    };
}
