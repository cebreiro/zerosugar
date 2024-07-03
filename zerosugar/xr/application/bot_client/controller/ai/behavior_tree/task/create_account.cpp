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
        BotController& controller = *blackBoard.Get<BotController*>("owner");

        const auto id = std::format("bot{}", controller.GetId());

        using namespace network::login;

        cs::CreateAccount packet;
        packet.account = id;
        packet.password = id;

        controller.Send(Packet::ToBuffer(packet));

        std::variant<sc::CreateAccountResult> va = co_await bt::Event<sc::CreateAccountResult>();
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
