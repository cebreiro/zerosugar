#include "wait_for_spawn_monster_dead.h"

#include "zerosugar/xr/server/game/instance/ai/behavior_tree/event/spawner_event.h"

namespace zerosugar::xr::game
{
    auto WaitForSpawnMonsterDead::Run() -> bt::node::Result
    {
        co_await bt::Event<event::SpawnerMonsterDead>();

        co_return true;
    }

    auto WaitForSpawnMonsterDead::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(WaitForSpawnMonsterDead&, const pugi::xml_node&)
    {
    }
}
