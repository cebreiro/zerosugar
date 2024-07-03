#include "create_character.h"

#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/network/packet.h"
#include "zerosugar/xr/network/model/generated/lobby_cs_message.h"
#include "zerosugar/xr/network/model/generated/lobby_sc_message.h"

namespace zerosugar::xr::bot
{
    auto CreateCharacter::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>("owner");

        using namespace network::lobby;

        cs::CreateCharacter packet;
        packet.character.slot = 0;
        packet.character.name = std::format("bot{}", controller.GetId());
        packet.character.faceId = 120;
        packet.character.hairId = 110;

        controller.Send(Packet::ToBuffer(packet));

        std::variant<sc::ResultCreateCharacter> va = co_await bt::Event<sc::ResultCreateCharacter>();
        const sc::ResultCreateCharacter& result = std::get<sc::ResultCreateCharacter>(va);

        if (result.success)
        {
            if (auto* lobbyCharacters = blackBoard.GetIf<std::vector<LobbyCharacter>>("lobby_characters");
                lobbyCharacters)
            {
                lobbyCharacters->push_back(result.character);
            }
            else
            {
                blackBoard.Insert<std::vector<LobbyCharacter>>("lobby_characters", { result.character });
            }

            co_return true;
        }

        co_return false;
    }

    auto CreateCharacter::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(CreateCharacter&, const pugi::xml_node&)
    {
    }
}
