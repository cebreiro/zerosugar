#include "connect_to.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_xml.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/socket.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/event/socket_event.h"

namespace zerosugar::xr::bot
{
    auto ConnectTo::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>(BotController::name);

        if (controller.GetSocket().IsOpen())
        {
            co_return false;
        }

        const auto* address = blackBoard.GetIf<std::pair<std::string, int32_t>>(
            fmt::format("{}_address", _target));
        if (!address)
        {
            co_return false;
        }

        controller.ConnectTo(address->first, static_cast<uint16_t>(address->second), 3000)
            .Then(controller.GetStrand(), [controller = controller.shared_from_this()]()
                {
                    controller->InvokeOnBehaviorTree([](BehaviorTree& behaviorTree)
                        {
                            behaviorTree.Notify(event::SocketConnected {});
                        });
                });

        [[maybe_unused]]
        const auto va = co_await bt::Event<event::SocketConnected>();

        // TODO: fix
        if (_target.starts_with("login"))
        {
            controller.SetSessionState(BotSessionStateType::Login);
        }
        else if (_target.starts_with("lobby"))
        {
            controller.SetSessionState(BotSessionStateType::Lobby);
        }
        else if (_target.starts_with("game"))
        {
            controller.SetSessionState(BotSessionStateType::Game);
        }

        co_return true;
    }

    auto ConnectTo::GetName() const -> std::string_view
    {
        return name;
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
