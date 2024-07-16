#pragma once
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"

namespace zerosugar::xr::bot
{
    class Monster;
    class VisualObjectContainer;
}

namespace zerosugar::xr::bot
{
    class UpdateBattleTarget final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "update_battle_target";

        auto Run() -> bt::node::Result override;

        auto GetName() const->std::string_view override;

        friend void from_xml(UpdateBattleTarget&, const pugi::xml_node&);

    private:
        auto SelectByMinDistance(VisualObjectContainer& visualObjectContainer, const Eigen::Vector3d& localPos) -> Monster*;
        auto SelectByRandom(VisualObjectContainer& visualObjectContainer, std::mt19937& randomEngine) -> Monster*;
    };
}
