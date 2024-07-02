#include "login.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/network/packet.h"
#include "zerosugar/xr/network/model/generated/login_cs_message.h"
#include "zerosugar/xr/network/model/generated/login_sc_message.h"

namespace zerosugar::xr::bot
{
    auto Login::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = _bt->GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>("owner");

        const auto id = std::format("bot{}", controller.GetId());

        network::login::cs::Login packet;
        packet.account = id;
        packet.password = id;

        controller.Send(Packet::ToBuffer(packet));

        auto result = co_await bt::Event<network::login::sc::LoginResult>();

        if (const auto* ptr = std::get_if<network::login::sc::LoginResult>(&result); ptr)
        {
            const network::login::sc::LoginResult& loginResult = *ptr;
            if (loginResult.success)
            {
                blackBoard.Insert("auth_token", std::string(loginResult.authenticationToken));
                blackBoard.Insert("lobby_address", std::pair<std::string, int32_t>(loginResult.lobbyIp, loginResult.lobbyPort));

                co_return true;
            }
        }

        co_return false;
    }

    void from_xml(Login&, const pugi::xml_node&)
    {
    }
}
