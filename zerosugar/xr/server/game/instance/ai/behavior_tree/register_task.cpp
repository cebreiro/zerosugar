#include "register_task.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_serializer.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task/time_elapsed_after_movement.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task/start_move_to_random_position.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task/select_attack_target.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task/spawn_monster.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task/suspend_for_seconds.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task/suspend_until_spawn_monster_dead.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task/suspend_until_player_in_view.h"

namespace zerosugar::xr::ai
{
    void RegisterTask(bt::NodeSerializer& serializer)
    {
        serializer.RegisterXML<TimeElapsedAfterMovement>();
        serializer.RegisterXML<StartMoveToRandomPosition>();
        serializer.RegisterXML<SelectAttackTarget>();
        serializer.RegisterXML<SpawnMonster>();
        serializer.RegisterXML<SuspendForSeconds>();
        serializer.RegisterXML<SuspendUntilSpawnMonsterDead>();
        serializer.RegisterXML<SuspendUntilPlayerInView>();
    }
}
