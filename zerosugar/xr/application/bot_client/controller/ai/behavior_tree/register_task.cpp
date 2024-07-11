#include "register_task.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_serializer.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task/authenticate_lobby.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task/close_socket.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task/connect_to.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task/create_account.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task/create_character.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task/is_connected_to_login.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task/login.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task/select_character.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task/shutdown_behavior_tree.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task/transition_behavior_tree.h"

namespace zerosugar::xr::bot
{
    void RegisterTask(bt::NodeSerializer& serializer)
    {
        serializer.RegisterXML<AuthenticateLobby>();
        serializer.RegisterXML<CloseSocket>();
        serializer.RegisterXML<ConnectTo>();
        serializer.RegisterXML<CreateAccount>();
        serializer.RegisterXML<CreateCharacter>();
        serializer.RegisterXML<IsConnectedToLogin>();
        serializer.RegisterXML<Login>();
        serializer.RegisterXML<SelectCharacter>();
        serializer.RegisterXML<ShutdownBehaviorTree>();
        serializer.RegisterXML<TransitionBehaviorTree>();
    }
}
