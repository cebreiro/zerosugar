#include "player_move_handler.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/client/game_client.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/task/impl/entity_position_update.h"

namespace zerosugar::xr
{
    auto PlayerMoveHandler::HandlePacket(GameServer& server, Session& session, const network::game::cs::PlayerMove& packet)
        -> Future<void>
    {
        SharedPtrNotNull<GameClient> client = server.FindClient(session.GetId());
        if (!client)
        {
            session.Close();

            co_return;
        }

        std::shared_ptr<GameInstance> instance = client->GetGameInstance();
        if (!instance)
        {
            session.Close();

            co_return;
        }

        auto task = std::make_unique<game_task::EntityPositionUpdate>(this->ReleasePacket(), client->GetGameEntityId());

        instance->Summit(std::move(task), client->GetControllerId());

        co_return;
    }
}
