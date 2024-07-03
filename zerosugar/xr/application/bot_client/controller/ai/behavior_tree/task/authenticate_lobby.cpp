#include "authenticate_lobby.h"

#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/network/packet.h"
#include "zerosugar/xr/network/model/generated/lobby_cs_message.h"
#include "zerosugar/xr/network/model/generated/lobby_sc_message.h"

namespace zerosugar::xr::bot
{
    auto AuthenticateLobby::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>("owner");

        const std::string* authToken = blackBoard.GetIf<std::string>("auth_token");
        if (!authToken)
        {
            co_return false;
        }

        using namespace network::lobby;

        cs::Authenticate packet;
        packet.authenticationToken = *authToken;

        controller.Send(Packet::ToBuffer(packet));

        auto va = co_await bt::Event<sc::NotifyCharacterList, sc::FailAuthenticate>();

        const bool result = std::visit([&]<typename T>(const T& va) -> bool
            {
                if constexpr (std::is_same_v<T, sc::NotifyCharacterList>)
                {
                    if (va.count > 0)
                    {
                        [[maybe_unused]]
                        const bool added = blackBoard.Insert<std::vector<LobbyCharacter>>("lobby_characters", va.character);
                        assert(added);
                    }

                    return true;
                }
                else if constexpr (std::is_same_v<T, sc::FailAuthenticate>)
                {
                    return false;
                }
                else
                {
                    static_assert(!sizeof(T), "not implemented");

                    return false;
                }

            }, va);

        co_return result;
    }

    auto AuthenticateLobby::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(AuthenticateLobby&, const pugi::xml_node&)
    {
    }
}
