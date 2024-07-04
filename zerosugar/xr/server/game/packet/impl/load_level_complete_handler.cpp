#include "load_level_complete_handler.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/client/game_client.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_activate.h"

namespace zerosugar::xr
{
    using network::game::cs::LoadLevelComplete;

    auto LoadLevelCompleteHandler::HandlePacket(GameServer& server, Session& session,
        const LoadLevelComplete& packet) -> Future<void>
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

        auto task = std::make_unique<game_task::PlayerActivate>(client->GetGameEntityId());

        instance->Summit(std::move(task), client->GetControllerId());
    }
}
