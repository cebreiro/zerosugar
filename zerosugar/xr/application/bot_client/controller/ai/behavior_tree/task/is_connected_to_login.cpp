#include "is_connected_to_login.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/network/socket.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"

namespace zerosugar::xr::bot
{
    auto IsConnectedToLogin::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = _bt->GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>("owner");

        if (controller.GetSessionState() == BotSessionStateType::Login && controller.GetSocket().IsOpen())
        {
            return true;
        }

        return false;
    }

    void from_xml(IsConnectedToLogin&, const pugi::xml_node&)
    {
    }
}
