#include "move_player_handler.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/controller/game_client.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_move.h"

namespace zerosugar::xr
{
    auto MovePlayerHandler::HandlePacket(GameServer& server, Session& session, const network::game::cs::MovePlayer& packet)
        -> Future<void>
    {
        SharedPtrNotNull<GameClient> client = server.FindClient(session.GetId());
        if (!client)
        {
            session.Close();

            co_return;
        }

        const std::shared_ptr<GameInstance> instance = client->GetGameInstance();
        if (!instance)
        {
            session.Close();

            co_return;
        }

        auto task = std::make_unique<game_task::PlayerMove>(this->ReleasePacket(), client->GetGameEntityId());

        instance->Summit(std::move(task), client->GetControllerId());

        co_return;
    }
}
