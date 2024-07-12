#include "register_task.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_serializer.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task.hpp"

namespace zerosugar::xr::bot
{
    void RegisterTask(bt::NodeSerializer& serializer)
    {
        serializer.RegisterXML<AttackTarget>();
        serializer.RegisterXML<AuthenticateGame>();
        serializer.RegisterXML<AuthenticateLobby>();
        serializer.RegisterXML<BattleTargetWithin>();
        serializer.RegisterXML<CloseSocket>();
        serializer.RegisterXML<ConnectTo>();
        serializer.RegisterXML<CreateAccount>();
        serializer.RegisterXML<CreateCharacter>();
        serializer.RegisterXML<DungeonMatch>();
        serializer.RegisterXML<IsConnectedToLogin>();
        serializer.RegisterXML<Login>();
        serializer.RegisterXML<MoveToBattleTarget>();
        serializer.RegisterXML<SelectCharacter>();
        serializer.RegisterXML<ShutdownBehaviorTree>();
        serializer.RegisterXML<StartMoveToRandomPosition>();
        serializer.RegisterXML<TimeElapsedAfterMovement>();
        serializer.RegisterXML<TransitionBehaviorTree>();
        serializer.RegisterXML<UpdateBattleTarget>();
    }
}
