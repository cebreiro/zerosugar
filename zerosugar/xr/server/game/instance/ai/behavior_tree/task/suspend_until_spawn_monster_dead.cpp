#include "suspend_until_spawn_monster_dead.h"

#include "zerosugar/xr/server/game/instance/ai/behavior_tree/event/spawner_event.h"

namespace zerosugar::xr::ai
{
    auto SuspendUntilSpawnMonsterDead::Run() -> bt::node::Result
    {
        co_await bt::Event<event::SpawnerMonsterDead>();

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
