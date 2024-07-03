#include "select_character.h"

#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/network/packet.h"
#include "zerosugar/xr/network/model/generated/lobby_cs_message.h"
#include "zerosugar/xr/network/model/generated/lobby_sc_message.h"

namespace zerosugar::xr::bot
{
    auto SelectCharacter::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>("owner");

        using namespace network::lobby;

        const auto* lobbyCharacters = blackBoard.GetIf<std::vector<LobbyCharacter>>("lobby_characters");
        if (!lobbyCharacters)
        {
            co_return false;
        }

        const auto iter = std::ranges::min_element(*lobbyCharacters,
            [](const LobbyCharacter& lhs, const LobbyCharacter& rhs)
            {
                return lhs.slot < rhs.slot;
            });
        if (iter == lobbyCharacters->end())
        {
            co_return false;
        }

        cs::SelectCharacter packet;
        packet.slot = iter->slot;

        controller.Send(Packet::ToBuffer(packet));

        const std::variant<sc::SuccessSelectCharacter> va = co_await bt::Event<sc::SuccessSelectCharacter>();
        const sc::SuccessSelectCharacter& result = std::get<sc::SuccessSelectCharacter>(va);

        [[maybe_unused]]
        const bool success = blackBoard.Insert("game_address", std::pair<std::string, int32_t>(result.ip, result.port));
        assert(success);

        co_return true;
    }

    auto SelectCharacter::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(SelectCharacter&, const pugi::xml_node&)
    {
    }
}
