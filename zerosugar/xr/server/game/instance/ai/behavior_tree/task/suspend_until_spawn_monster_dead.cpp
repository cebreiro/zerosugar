#include "suspend_until_spawn_monster_dead.h"

#include "zerosugar/xr/network/model/generated/game_sc_message.h"

namespace zerosugar::xr::ai
{
    auto SuspendUntilSpawnMonsterDead::Run() -> bt::node::Result
    {
        co_await bt::Event<network::game::sc::SpawnerMonsterDead>();

        co_return true;
    }

    auto SuspendUntilSpawnMonsterDead::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(SuspendUntilSpawnMonsterDead&, const pugi::xml_node&)
    {
    }
}
