#include "player_move_handler.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/client/game_client.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr
{
    class EntityPositionUpdateTask : public GameTaskT<IPacket, network::game::cs::PlayerMove, game_task::MainTargetSelector>
    {
    public:
        EntityPositionUpdateTask(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId)
            : GameTaskT(std::chrono::system_clock::now(), std::move(param), game_task::MainTargetSelector(targetId))
        {
        }

    private:
        void OnComplete(GameInstance& gameInstance) override
        {
            (void)gameInstance;

            // gameInstance.GetSpatialContainer();
        }

    public:
        void Execute(GameInstance& gameInstance, game_task::MainTargetSelector::target_type target) override
        {
            (void)gameInstance;

            _id = target->GetId();

            const network::game::cs::PlayerMove& param = GetParam();
            auto& movementComponent = target->GetComponent<MovementComponent>();

            _newPosition.x() = param.position.x;
            _newPosition.y() = param.position.y;
            _newPosition.z() = param.position.z;

            movementComponent.SetPosition(_newPosition);
        }

    private:
        game_entity_id_type _id;
        Eigen::Vector3d _newPosition;
    };

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

        auto task = std::make_unique<EntityPositionUpdateTask>(std::move(this->_packet), client->GetGameEntityId());

        instance->Summit(std::move(task), client->GetControllerId());

        co_return;
    }
}
