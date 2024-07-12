#include "login.h"

#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/network/packet.h"
#include "zerosugar/xr/network/model/generated/login_cs_message.h"
#include "zerosugar/xr/network/model/generated/login_sc_message.h"

namespace zerosugar::xr::bot
{
    auto Login::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>(BotController::name);

        const auto id = std::format("bot{}", controller.GetId());

        using namespace network::login;

        cs::Login packet;
        packet.account = id;
        packet.password = id;

        controller.SendToServer(Packet::ToBuffer(packet));

        std::variant<sc::LoginResult> va = co_await bt::Event<sc::LoginResult>();
        const sc::LoginResult& result = std::get<sc::LoginResult>(va);

        if (result.success)
        {
            blackBoard.Insert("auth_token", std::string(result.authenticationToken));
            blackBoard.Insert("lobby_address", std::pair<std::string, int32_t>(result.lobbyIp, result.lobbyPort));

            co_return true;
        }

        co_return false;
    }

    auto Login::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(Login&, const pugi::xml_node&)
    {
    }
}
