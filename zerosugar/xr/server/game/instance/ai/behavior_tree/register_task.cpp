#include "register_task.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_serializer.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task/select_attack_target.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task/spawn_monster.h"

namespace zerosugar::xr::game
{
    void RegisterTask(bt::NodeSerializer& serializer)
    {
        serializer.RegisterXML<SelectAttackTarget>();
        serializer.RegisterXML<SpawnMonster>();
    }
}
