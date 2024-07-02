#include "connect_to.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_xml.h"
#include "zerosugar/shared/network/socket.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/event/connect_event.h"

namespace zerosugar::xr::bot
{
    auto ConnectTo::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = _bt->GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>("owner");

        if (controller.GetSocket().IsOpen())
        {
            co_return false;
        }

        const auto* address = blackBoard.GetIf<std::pair<std::string, int32_t>>(_target);
        if (!address)
        {
            co_return false;
        }

        controller.ConnectTo(address->first, static_cast<uint16_t>(address->second), 3000);

        co_await bt::Event<event::OnSessionConnected>();

        co_return true;
    }

    void from_xml(ConnectTo& self, const pugi::xml_node& node)
    {
        if (auto attr = node.attribute("target"); attr)
        {
            self._target = attr.as_string();
        }
        else
        {
            assert(false);
        }
    }
}
