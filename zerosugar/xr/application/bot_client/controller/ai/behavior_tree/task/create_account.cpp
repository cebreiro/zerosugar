#include "create_account.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/network/packet.h"
#include "zerosugar/xr/network/model/generated/login_cs_message.h"
#include "zerosugar/xr/network/model/generated/login_sc_message.h"


namespace zerosugar::xr::bot
{
    auto CreateAccount::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = _bt->GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>("owner");

        const auto id = std::format("bot{}", controller.GetId());

        network::login::cs::CreateAccount packet;
        packet.account = id;
        packet.password = id;

        controller.Send(Packet::ToBuffer(packet));

        auto result = co_await bt::Event<network::login::sc::CreateAccountResult>();

        if (const auto* ptr = std::get_if<network::login::sc::CreateAccountResult>(&result); ptr)
        {
            const network::login::sc::CreateAccountResult& asd = *ptr;

            co_return asd.success;
        }

        co_return false;
    }

    void from_xml(CreateAccount&, const pugi::xml_node&)
    {
    }
}
