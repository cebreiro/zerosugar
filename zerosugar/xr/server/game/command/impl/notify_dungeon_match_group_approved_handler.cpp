#include "notify_dungeon_match_group_approved_handler.h"

#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/client/game_client.h"
#include "zerosugar/xr/server/game/client/game_client_container.h"
#include "zerosugar/xr/server/game/instance/game_instance_container.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"

namespace zerosugar::xr
{
    using coordination::command::NotifyDungeonMatchGroupApproved;

    auto NotifyDungeonMatchGroupApprovedHandler::HandleCommand(GameServer& server,
        NotifyDungeonMatchGroupApproved command, [[maybe_unused]] std::optional<int64_t> responseId) const -> Future<void>
    {
        assert(!responseId.has_value());

        std::shared_ptr<GameClient> client = server.GetClientContainer().FindByUserId(command.userId);
        if (!client)
        {
            co_return;
        }

        co_await server.ShutdownClient(command.userId);

        network::game::sc::NotifyDungeonMatchGroupApproved packet;
        packet.ip = command.ip;
        packet.port = command.port;

        client->Notify(packet);
    }
}
