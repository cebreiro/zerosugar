#include "authenticate_handler.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/client/game_client.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/game_instance_container.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_serializer_interface.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_spawn.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"
#include "zerosugar/xr/service/model/generated/coordination_service_message.h"
#include "zerosugar/xr/service/model/generated/database_service.h"

namespace zerosugar::xr
{
    auto AuthenticateHandler::HandlePacket(GameServer& server, Session& session,
        const network::game::cs::Authenticate& packet) -> Future<void>
    {
        using namespace service;

        ServiceLocator& serviceLocator = server.GetServiceLocator();
        auto& coordinationService = serviceLocator.Get<ICoordinationService>();
        auto& databaseService = serviceLocator.Get<IDatabaseService>();

        if (server.HasClient(session.GetId()))
        {
            ZEROSUGAR_LOG_WARN(server.GetServiceLocator(),
                std::format("[{}] authenticate - invalid request. session: {}, error: {}", server.GetName(), session));

            session.Close();

            co_return;
        }

        AuthenticatePlayerParam authParam;
        authParam.serverId = server.GetServerId();
        authParam.authenticationToken = packet.authenticationToken;

        AuthenticatePlayerResult authResult = co_await coordinationService.AuthenticatePlayerAsync(authParam);
        if (authResult.errorCode != CoordinationServiceErrorCode::CoordinationErrorNone)
        {
            ZEROSUGAR_LOG_WARN(server.GetServiceLocator(),
                std::format("[{}] fail to authenticate session. session: {}, error: {}",
                    server.GetName(), session, GetEnumName(authResult.errorCode)));

            session.Close();

            co_return;
        }

        GetCharacterParam getCharacterParam;
        getCharacterParam.characterId = authResult.characterId;

        GetCharacterResult getCharacterResult = co_await databaseService.GetCharacterAsync(getCharacterParam);
        if (getCharacterResult.errorCode != DatabaseServiceErrorCode::DatabaseErrorNone)
        {
            // TODO: remove coordination auth

            ZEROSUGAR_LOG_CRITICAL(server.GetServiceLocator(),
                std::format("[{}] fail to get character data fro database service. session: {}, error: {}",
                    server.GetName(), session, GetEnumName(getCharacterResult.errorCode)));

            session.Close();

            co_return;
        }

        SharedPtrNotNull<GameInstance> gameInstance =
            server.GetGameInstanceContainer().Find(game_instance_id_type(authResult.gameInstanceId));
        if (!gameInstance)
        {
            ZEROSUGAR_LOG_CRITICAL(server.GetServiceLocator(),
                std::format("[{}] fail to find game instance. session: {}",
                    server.GetName(), session, GetEnumName(authResult.errorCode)));

            session.Close();

            co_return;
        }

        auto client = std::make_shared<GameClient>(session.weak_from_this(),
            authParam.authenticationToken, authResult.accountId, authResult.characterId, gameInstance);

        [[maybe_unused]]
        const bool added = server.AddClient(session.GetId(), client);
        assert(added);

        const game_controller_id_type controllerId = gameInstance->PublishControllerId();
        const game_entity_id_type entityId = gameInstance->PublishPlayerId();

        SharedPtrNotNull<GameEntity> entity = server.GetGameEntitySerializer().Deserialize(getCharacterResult.character);
        entity->SetId(entityId);
        entity->SetController(client);

        client->SetGameInstance(gameInstance);
        client->SetControllerId(controllerId);
        client->SetGameEntityId(entity->GetId());

        auto task = std::make_unique<game_task::PlayerSpawn>(std::move(entity));
        gameInstance->Summit(std::move(task), controllerId);

        co_return;
    }
}
