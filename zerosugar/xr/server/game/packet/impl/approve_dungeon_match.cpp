#include "approve_dungeon_match.h"

#include "zerosugar/shared/execution/executor/operation/schedule.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/packet.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/client/game_client.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"

namespace zerosugar::xr
{
    using network::game::cs::ApproveDungeonMatch;

    auto ApproveDungeonMatchHandler::HandlePacket(GameServer& server, Session& session,
        UniquePtrNotNull<ApproveDungeonMatch> packet) -> Future<void>
    {
        SharedPtrNotNull<GameClient> client = server.FindClient(session.GetId());
        if (!client)
        {
            ZEROSUGAR_LOG_WARN(server.GetServiceLocator(),
                fmt::format("[approve_dungeon_match_handler] fail to find client. session: {}",
                    session.GetId()));

            session.Close();

            co_return;
        }

        const std::shared_ptr<GameInstance> instance = client->GetGameInstance();
        if (!instance)
        {
            ZEROSUGAR_LOG_WARN(server.GetServiceLocator(),
                fmt::format("[approve_dungeon_match_handler] fail to find game instance. client_cid: {}",
                    client->GetCharacterId()));

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
                fmt::format("[approve_dungeon_match_handler] fail to start. session: {}, error: {}",
                    session, GetEnumName(result.errorCode)));

            const network::game::sc::NotifyDungeonMatchFailure failure;
            session.Send(Packet::ToBuffer(failure));
        }

        co_return;
    }
}
