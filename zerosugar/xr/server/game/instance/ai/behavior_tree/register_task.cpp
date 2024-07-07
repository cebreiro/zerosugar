#include "register_task.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_serializer.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task/monster_roam.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task/select_attack_target.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task/spawn_monster.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task/wait_for_seconds.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task/wait_for_spawn_monster_dead.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task/wait_until_player_in_view.h"

namespace zerosugar::xr::game
{
    void RegisterTask(bt::NodeSerializer& serializer)
    {
        serializer.RegisterXML<MonsterRoam>();
        serializer.RegisterXML<SelectAttackTarget>();
        serializer.RegisterXML<SpawnMonster>();
        serializer.RegisterXML<WaitForSeconds>();
        serializer.RegisterXML<WaitForSpawnMonsterDead>();
        serializer.RegisterXML<WaitUntilPlayerInView>();
    }
}
