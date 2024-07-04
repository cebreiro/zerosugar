#include "approve_dungeon_match.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/packet.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/client/game_client.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"

namespace zerosugar::xr
{
    auto ApproveDungeonMatchHandler::HandlePacket(GameServer& server, Session& session,
        const network::game::cs::ApproveDungeonMatch& packet) -> Future<void>
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

        service::ApproveDungeonMatchParam param;
        param.serverId = server.GetServerId();
        param.gameInstanceId = instance->GetId().Unwrap();
        param.authenticationToken = client->GetAuthenticationToken();

        service::ApproveDungeonMatchResult result = co_await server.GetServiceLocator().Get<service::ICoordinationService>()
            .ApproveDungeonMatchAsync(std::move(param));

        if (result.errorCode != service::CoordinationServiceErrorCode::CoordinationErrorNone)
        {
            ZEROSUGAR_LOG_WARN(server.GetServiceLocator(),
                std::format("[approve_dungeon_match_handler] fail to start. session: {}, error: {}",
                    session, GetEnumName(result.errorCode)));

            const network::game::sc::NotifyDungeonMatchFailure failure;
            session.Send(Packet::ToBuffer(failure));
        }

        co_return;
    }
}
