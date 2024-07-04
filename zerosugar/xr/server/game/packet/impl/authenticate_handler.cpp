#include "authenticate_handler.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/client/game_client.h"
#include "zerosugar/xr/server/game/repository/game_repository_interface.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/game_instance_container.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_serializer_interface.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_spawn.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"
#include "zerosugar/xr/service/model/generated/coordination_service_message.h"

namespace zerosugar::xr
{
    auto AuthenticateHandler::HandlePacket(GameServer& server, Session& session,
        const network::game::cs::Authenticate& packet) -> Future<void>
    {
        using namespace service;

        ServiceLocator& serviceLocator = server.GetServiceLocator();

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

        AuthenticatePlayerResult authResult = co_await serviceLocator.Get<ICoordinationService>().AuthenticatePlayerAsync(authParam);
        if (authResult.errorCode != CoordinationServiceErrorCode::CoordinationErrorNone)
        {
            ZEROSUGAR_LOG_WARN(server.GetServiceLocator(),
                std::format("[{}] fail to authenticate session. session: {}, error: {}",
                    server.GetName(), session, GetEnumName(authResult.errorCode)));

            session.Close();

            co_return;
        }

        IGameRepository& repository = serviceLocator.Get<IGameRepository>();
        std::optional<DTOCharacter> character = co_await repository.Find(authResult.characterId);

        if (!character.has_value())
        {
            // TODO: remove auth

            ZEROSUGAR_LOG_CRITICAL(server.GetServiceLocator(),
                std::format("[{}] fail to find character from repository. session: {}, auth_token: {}, character_id: {}",
                    server.GetName(), session, authParam.authenticationToken, authResult.characterId));

            session.Close();

            co_return;
        }

        SharedPtrNotNull<GameInstance> gameInstance =
            server.GetGameInstanceContainer().Find(game_instance_id_type(authResult.gameInstanceId));
        if (!gameInstance)
        {
            // TODO: remove auth
            
            ZEROSUGAR_LOG_CRITICAL(server.GetServiceLocator(),
                std::format("[{}] fail to find game instance. session: {}, instance_id: {}",
                    server.GetName(), session, authResult.gameInstanceId));

            session.Close();

            co_return;
        }

        auto client = std::make_shared<GameClient>(session.weak_from_this(),
            authParam.authenticationToken, authResult.accountId, authResult.characterId, authResult.userUniqueId, gameInstance);

        [[maybe_unused]]
        const bool added = server.AddClient(session.GetId(), client);
        assert(added);

        const game_controller_id_type controllerId = gameInstance->PublishControllerId();
        const game_entity_id_type entityId = gameInstance->PublishEntityId(GameEntityType::Player);

        SharedPtrNotNull<GameEntity> entity = server.GetGameEntitySerializer().Deserialize(*character);
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
