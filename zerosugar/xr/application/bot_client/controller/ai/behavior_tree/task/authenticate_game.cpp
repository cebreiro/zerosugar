#include "authenticate_game.h"

#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/application/bot_client/controller/bot_control_service.h"
#include "zerosugar/xr/network/packet.h"
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"

namespace zerosugar::xr::bot
{
    auto AuthenticateGame::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>(BotController::name);

        const std::string* authToken = blackBoard.GetIf<std::string>("auth_token");
        if (!authToken)
        {
            ZEROSUGAR_LOG_ERROR(controller.GetServiceLocator(),
                fmt::format("[{}] fail to find auth token from black bloard",
                    controller.GetName()));

            co_return false;
        }

        using namespace network::game;

        cs::Authenticate authenticate;
        authenticate.authenticationToken = *authToken;

        controller.SendToServer(Packet::ToBuffer(authenticate));

        const auto va = co_await bt::Event<sc::EnterGame>();
        const sc::EnterGame& enterGame = std::get<sc::EnterGame>(va);

        if (enterGame.zoneId != 100)
        {
            cs::LoadLevelComplete loadLevelComplete;
            controller.SendToServer(Packet::ToBuffer(loadLevelComplete));
        }

        (void)co_await bt::Event<sc::NotifyPlayerActivated>();

        co_return true;
    }

    auto AuthenticateGame::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(AuthenticateGame&, const pugi::xml_node&)
    {
    }
}
