#include "register_task.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_serializer.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/task.hpp"

namespace zerosugar::xr::ai
{
    void RegisterTask(bt::NodeSerializer& serializer)
    {
        serializer.RegisterXML<BattleTargetWithin>();
        serializer.RegisterXML<DetectPlayerAround>();
        serializer.RegisterXML<DetectUnderAttack>();
        serializer.RegisterXML<MoveToBattleTarget>();
        serializer.RegisterXML<ReturnToPrevBehaviorTree>();
        serializer.RegisterXML<SpawnMonster>();
        serializer.RegisterXML<StartMoveToRandomPosition>();
        serializer.RegisterXML<SuspendForSeconds>();
        serializer.RegisterXML<SuspendUntilPlayerInView>();
        serializer.RegisterXML<SuspendUntilSpawnMonsterDead>();
        serializer.RegisterXML<TimeElapsedAfterMovement>();
        serializer.RegisterXML<TransitionBehaviorTree>();
        serializer.RegisterXML<UpdateBattleTarget>();
        serializer.RegisterXML<UseRandomSkill>();
    }
}
