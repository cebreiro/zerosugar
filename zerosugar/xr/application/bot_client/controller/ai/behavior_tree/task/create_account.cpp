#include "create_account.h"

#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/network/packet.h"
#include "zerosugar/xr/network/model/generated/login_cs_message.h"
#include "zerosugar/xr/network/model/generated/login_sc_message.h"

namespace zerosugar::xr::bot
{
    auto CreateAccount::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>(BotController::name);

        const auto id = fmt::format("bot{}", controller.GetId());

        using namespace network::login;

        const auto va = co_await bt::Event<sc::CreateAccountResult>([&]()
            {
                cs::CreateAccount packet;
                packet.account = id;
                packet.password = id;

                controller.SendToServer(Packet::ToBuffer(packet));
            });
        const sc::CreateAccountResult& result = std::get<sc::CreateAccountResult>(va);

        co_return result.success;
    }

    auto CreateAccount::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(CreateAccount&, const pugi::xml_node&)
    {
    }
}
