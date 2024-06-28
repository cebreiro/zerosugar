#include "entity_position_update.h"

#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/controller/game_entity_controller_interface.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/view/game_view_model_container.h"
#include "zerosugar/xr/server/game/instance/view/game_player_view_model.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/view/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/packet/packet_builder.h"

namespace zerosugar::xr::game_task
{
    EntityPositionUpdate::EntityPositionUpdate(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId)
        : GameTaskT(std::chrono::system_clock::now(), std::move(param), MainTargetSelector(targetId))
    {
    }

    void EntityPositionUpdate::Execute(GameExecutionParallel& parallel, MainTargetSelector::target_type target)
    {
        (void)parallel;

        _id = target->GetId();

        const network::game::cs::MovePlayer& param = GetParam();
        auto& movementComponent = target->GetComponent<MovementComponent>();

        _oldPosition = movementComponent.GetPosition();

        _newPosition.x() = param.position.x;
        _newPosition.y() = param.position.y;
        _newPosition.z() = param.position.z;

        movementComponent.SetPosition(_newPosition);
    }

    void EntityPositionUpdate::OnComplete(GameExecutionSerial& serial)
    {
        GameViewModelContainer& viewContainer = serial.GetEntityViewContainer();

        GamePlayerViewModel* view = viewContainer.FindPlayer(_id);
        if (!view)
        {
            return;
        }

        view->SetPosition(_newPosition);

        GameSpatialContainer& spatialContainer = serial.GetSpatialContainer();

        GameSpatialSector& oldSector = spatialContainer.GetSector(_oldPosition.x(), _oldPosition.y());
        GameSpatialSector& newSector = spatialContainer.GetSector(_newPosition.x(), _newPosition.y());
        {
            if (oldSector.GetId() == newSector.GetId())
            {
                if (auto range = oldSector.GetEntities(GameEntityType::Player); range.begin() != range.end())
                {
                    network::game::sc::MoveRemotePlayer packet;
                    GamePacketBuilder::Build(packet, *view);

                    for (const game_entity_id_type id : range)
                    {
                        if (id == _id)
                        {
                            continue;
                        }

                        GamePlayerViewModel* remote = viewContainer.FindPlayer(id);
                        assert(remote);

                        if (IGameController& controller = remote->GetController();
                            controller.IsSubscriberOf(decltype(packet)::opcode))
                        {
                            controller.Notify(packet);
                        }
                    }
                }

                return;
            }

            oldSector.RemoveEntity(_id);
            GameSpatialSector::Subset outs = (oldSector - newSector);

            if (auto range = outs.GetEntities(GameEntityType::Player); range.begin() != range.end())
            {
                network::game::sc::RemoveRemotePlayer packet;
                GamePacketBuilder::Build(packet, *view);

                for (const game_entity_id_type id : range)
                {
                    GamePlayerViewModel* remote = viewContainer.FindPlayer(id);
                    assert(remote);

                    if (IGameController& controller = remote->GetController();
                        controller.IsSubscriberOf(decltype(packet)::opcode))
                    {
                        controller.Notify(packet);
                    }
                }
            }

            GameSpatialSector::Subset unchanged = (newSector & oldSector);
            if (auto range = unchanged.GetEntities(GameEntityType::Player);
                range.begin() != range.end() && ++range.begin() != range.end())
            {
                network::game::sc::MoveRemotePlayer packet;
                GamePacketBuilder::Build(packet, *view);

                for (const game_entity_id_type id : range)
                {
                    GamePlayerViewModel* remote = viewContainer.FindPlayer(id);
                    assert(remote);

                    if (IGameController& controller = remote->GetController();
                        controller.IsSubscriberOf(decltype(packet)::opcode))
                    {
                        controller.Notify(packet);
                    }
                }
            }

            GameSpatialSector::Subset ins = (newSector - oldSector);
            if (auto range = ins.GetEntities(GameEntityType::Player); range.begin() != range.end())
            {
                network::game::sc::AddRemotePlayer packet;
                GamePacketBuilder::Build(packet, *view);

                for (const game_entity_id_type id : range)
                {
                    GamePlayerViewModel* remote = viewContainer.FindPlayer(id);
                    assert(remote);

                    if (IGameController& controller = remote->GetController();
                        controller.IsSubscriberOf(decltype(packet)::opcode))
                    {
                        controller.Notify(packet);
                    }
                }
            }

            newSector.AddEntity(_id);
        }
    }
}
