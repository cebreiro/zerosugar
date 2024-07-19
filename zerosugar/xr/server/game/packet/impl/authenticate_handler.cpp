#include "authenticate_handler.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/navigation/navigation_service.h"
#include "zerosugar/xr/navigation/navi_vector.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/client/game_client.h"
#include "zerosugar/xr/server/game/instance/game_constants.h"
#include "zerosugar/xr/server/game/repository/game_repository_interface.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/game_instance_container.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_serializer_interface.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_spawn.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"
#include "zerosugar/xr/service/model/generated/coordination_service_message.h"

namespace zerosugar::xr
{
    using network::game::cs::Authenticate;

    auto AuthenticateHandler::HandlePacket(GameServer& server, Session& session,
        UniquePtrNotNull<Authenticate> packet) -> Future<void>
    {
        using namespace service;

        ServiceLocator& serviceLocator = server.GetServiceLocator();

        if (server.HasClient(session.GetId()))
        {
            ZEROSUGAR_LOG_WARN(server.GetServiceLocator(),
                fmt::format("[{}] authenticate - invalid request. session: {}",
                    server.GetName(), session));

            session.Close();

            co_return;
        }

        ICoordinationService& coordinationService = serviceLocator.Get<ICoordinationService>();

        AuthenticatePlayerParam authParam;
        authParam.serverId = server.GetServerId();
        authParam.authenticationToken = packet->authenticationToken;

        AuthenticatePlayerResult authResult = co_await coordinationService.AuthenticatePlayerAsync(authParam);
        if (authResult.errorCode != CoordinationServiceErrorCode::CoordinationErrorNone)
        {
            ZEROSUGAR_LOG_WARN(server.GetServiceLocator(),
                fmt::format("[{}] fail to authenticate session. session: {}, error: {}",
                    server.GetName(), session, GetEnumName(authResult.errorCode)));

            session.Close();

            co_return;
        }

        do
        {
            IGameRepository& repository = serviceLocator.Get<IGameRepository>();
            std::optional<DTOCharacter> character = co_await repository.Find(authResult.characterId);

            if (!character.has_value())
            {
                // TODO: remove auth

                ZEROSUGAR_LOG_CRITICAL(server.GetServiceLocator(),
                    fmt::format("[{}] fail to find character from repository. session: {}, auth_token: {}, character_id: {}",
                        server.GetName(), session, authParam.authenticationToken, authResult.characterId));

                session.Close();

                break;
            }

            SharedPtrNotNull<GameInstance> gameInstance =
                server.GetGameInstanceContainer().Find(game_instance_id_type(authResult.gameInstanceId));
            if (!gameInstance)
            {
                // TODO: remove auth

                ZEROSUGAR_LOG_CRITICAL(server.GetServiceLocator(),
                    fmt::format("[{}] fail to find game instance. session: {}, instance_id: {}",
                        server.GetName(), session, authResult.gameInstanceId));

                session.Close();

                break;
            }

            auto client = std::make_shared<GameClient>(session.shared_from_this(),
                authParam.authenticationToken, authResult.accountId, authResult.characterId, authResult.userUniqueId, gameInstance);

            [[maybe_unused]]
            const bool added = server.AddClient(session.GetId(), client);
            assert(added);

            const game_controller_id_type controllerId = gameInstance->PublishControllerId();
            const game_entity_id_type entityId = gameInstance->PublishEntityId(GameEntityType::Player);

            SharedPtrNotNull<GameEntity> entity = gameInstance->GetEntitySerializer().Deserialize(*character);
            entity->SetId(entityId);
            entity->SetController(client);

            client->SetGameInstance(gameInstance);
            client->SetControllerId(controllerId);
            client->SetGameEntityId(entity->GetId());

            if (auto naviation = gameInstance->GetNavigationService(); naviation)
            {
                MovementComponent& movementComponent = entity->GetComponent<MovementComponent>();

                std::vector<navi::FVector> randPos = co_await naviation->GetRandomPoints(1);
                if (!randPos.empty())
                {
                    movementComponent.SetPosition(Eigen::Vector3d(randPos[0].GetX(), randPos[0].GetY(), randPos[0].GetZ()));
                }
            }

            auto task = std::make_unique<game_task::PlayerSpawn>(std::move(entity));
            gameInstance->Summit(std::move(task), controllerId);

            co_return;
            
        } while (false);


        service::RemovePlayerParam param;
        param.serverId = server.GetServerId();
        param.authenticationToken = authParam.authenticationToken;

        RemovePlayerResult removeResult = co_await coordinationService.RemovePlayerAsync(std::move(param));
        if (removeResult.errorCode != CoordinationServiceErrorCode::CoordinationErrorNone)
        {
            ZEROSUGAR_LOG_ERROR(server.GetServiceLocator(),
                fmt::format("[{}] fail to remove auth. auth_token: {}",
                    server.GetName(), authParam.authenticationToken));
        }

        co_return;
    }
}
