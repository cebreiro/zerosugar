#include "cancel_dungeon_match.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/controller/game_client.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"

namespace zerosugar::xr
{
    auto CancelDungeonMatchHandler::HandlePacket(GameServer& server, Session& session,
        const network::game::cs::CancelDungeonMatch& packet) -> Future<void>
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

        service::CancelDungeonMatchParam param;
        param.serverId = server.GetServerId();
        param.gameInstanceId = instance->GetId().Unwrap();
        param.authenticationToken = client->GetAuthenticationToken();

        service::CancelDungeonMatchResult result = co_await server.GetServiceLocator().Get<service::ICoordinationService>()
            .CancelDungeonMatchAsync(std::move(param));

        // it can be failed. request cancel but already match group created.
        (void)result;

        co_return;
    }
}
