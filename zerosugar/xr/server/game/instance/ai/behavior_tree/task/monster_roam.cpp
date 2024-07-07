#include "monster_roam.h"

namespace zerosugar::xr::game
{
    auto MonsterRoam::Run() -> bt::node::Result
    {
        return true;
    }

    auto MonsterRoam::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(MonsterRoam&, const pugi::xml_node&)
    {
    }
}
