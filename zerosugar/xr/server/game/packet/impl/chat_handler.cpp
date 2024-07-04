#include "chat_handler.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/client/game_client.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_local_chat.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"

namespace zerosugar::xr
{
    auto ChatHandler::HandlePacket(GameServer& server, Session& session, const network::game::cs::Chat& packet) -> Future<void>
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

        if (packet.message.starts_with("//"))
        {
            std::string message = packet.message.substr(2);
            if (message.empty())
            {
                co_return;
            }

            auto& coordinationService = server.GetServiceLocator().Get<service::ICoordinationService>();

            service::BroadcastChattingParam param;
            param.serverId = server.GetServerId();
            param.gameInstanceId = instance->GetId().Unwrap();
            param.authenticationToken = client->GetAuthenticationToken();
            param.message = std::move(message);

            service::BroadcastChattingResult result = co_await coordinationService.BroadcastChattingAsync(param);
            if (result.errorCode != service::CoordinationServiceErrorCode::CoordinationErrorNone)
            {
                ZEROSUGAR_LOG_WARN(server.GetServiceLocator(),
                    std::format("[chat_handler] fail to broadcast chatting. server_id: {}, game_instance_id: {}, auth: {}, message: {}",
                        param.serverId, param.gameInstanceId, param.authenticationToken, param.message));
            }
        }
        else
        {
            auto task = std::make_unique<game_task::PlayerLocalChat>(ReleasePacket(), client->GetGameEntityId());

            instance->Summit(std::move(task), client->GetControllerId());
        }
    }
}
