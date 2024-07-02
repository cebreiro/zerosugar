#include "register.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_serializer.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task/connect_to.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task/create_account.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task/is_connected_to_login.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task/login.h"

namespace zerosugar::xr::bot
{
    void Register(bt::NodeSerializer& serializer)
    {
        serializer.RegisterXML<ConnectTo>();
        serializer.RegisterXML<CreateAccount>();
        serializer.RegisterXML<IsConnectedToLogin>();
        serializer.RegisterXML<Login>();
        
    }
}
