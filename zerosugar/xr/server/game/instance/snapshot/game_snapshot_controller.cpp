#include "game_snapshot_controller.h"

#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/controller/game_controller_interface.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_view.h"
#include "zerosugar/xr/server/game/instance/snapshot/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/grid/game_spatial_sector.h"
#include "zerosugar/xr/server/game/packet/packet_builder.h"

namespace zerosugar::xr
{
    GameSnapshotController::GameSnapshotController(GameInstance& instance)
        : _gameInstance(instance)
    {
    }

    GameSnapshotController::~GameSnapshotController()
    {
    }

    void GameSnapshotController::ProcessPlayerSpawn(const GameEntity& entity)
    {
        GameSnapshotModelContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(entity.GetId());
        assert(snapshot);

        GameSpatialSector& sector = spatialContainer.GetSector(snapshot->GetPosition().x(), snapshot->GetPosition().y());
        {
            network::game::sc::EnterGame packet;
            GamePacketBuilder::Build(packet, _gameInstance, entity, sector);

            view.Send(packet, snapshot->GetController());
        }

        if (!sector.Empty() && sector.HasEntitiesAtLeast(GameEntityType::Player, 0))
        {
            network::game::sc::AddRemotePlayer packet;
            GamePacketBuilder::Build(packet, *snapshot);

            view.Broadcast(packet, sector, GameEntityType::Player);
        }

        sector.AddEntity(snapshot->GetId());
    }

    void GameSnapshotController::ProcessMovement(game_entity_id_type id, const Eigen::Vector3d& position)
    {
        GameSnapshotModelContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(id);
        if (!snapshot)
        {
            return;
        }

        const Eigen::Vector3d oldPosition = snapshot->GetPosition();
        snapshot->SetPosition(position);

        GameSpatialSector& oldSector = spatialContainer.GetSector(oldPosition.x(), oldPosition.y());
        GameSpatialSector& newSector = spatialContainer.GetSector(position.x(), position.y());

        if (oldSector.GetId() == newSector.GetId())
        {
            if (!oldSector.HasEntitiesAtLeast(GameEntityType::Player, 1))
            {
                return;
            }

            network::game::sc::MoveRemotePlayer packet;
            GamePacketBuilder::Build(packet, *snapshot);

            view.Broadcast(packet, oldSector, id);

            return;
        }

        oldSector.RemoveEntity(id);

        if (GameSpatialSector::Subset outs = (oldSector - newSector);
            !outs.Empty() && outs.HasEntitiesAtLeast(GameEntityType::Player, 0))
        {
            network::game::sc::RemoveRemotePlayer packet;
            GamePacketBuilder::Build(packet, *snapshot);

            view.Broadcast(packet, outs, GameEntityType::Player);
        }

        if (GameSpatialSector::Subset unchanged = (newSector & oldSector);
            !unchanged.Empty() && unchanged.HasEntitiesAtLeast(GameEntityType::Player, 1))
        {
            network::game::sc::MoveRemotePlayer packet;
            GamePacketBuilder::Build(packet, *snapshot);

            view.Broadcast(packet, unchanged, GameEntityType::Player, id);
        }

        if (GameSpatialSector::Subset ins = (newSector - oldSector);
            !ins.Empty() && ins.HasEntitiesAtLeast(GameEntityType::Player, 0))
        {
            network::game::sc::AddRemotePlayer packet;
            GamePacketBuilder::Build(packet, *snapshot);

            view.Broadcast(packet, ins, GameEntityType::Player);
        }

        newSector.AddEntity(id);

        ZEROSUGAR_LOG_DEBUG(_gameInstance.GetServiceLocator(),
            std::format("movement: old[{}, {}] new[{}, [{}], old_sector[{}, {}], new_sector[{}, {}]",
                oldPosition.x(), oldPosition.y(),
                position.x(), position.y(),
                oldSector.GetId().GetX(), oldSector.GetId().GetY(),
                newSector.GetId().GetX(), newSector.GetId().GetY()));
    }

    void GameSnapshotController::ProcessStop(game_entity_id_type id, const Eigen::Vector3d& position)
    {
        GameSnapshotModelContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(id);
        if (!snapshot)
        {
            return;
        }

        const Eigen::Vector3d oldPosition = snapshot->GetPosition();
        snapshot->SetPosition(position);

        GameSpatialSector& oldSector = spatialContainer.GetSector(oldPosition.x(), oldPosition.y());
        GameSpatialSector& newSector = spatialContainer.GetSector(position.x(), position.y());

        if (oldSector.GetId() == newSector.GetId())
        {
            if (!oldSector.HasEntitiesAtLeast(GameEntityType::Player, 1))
            {
                return;
            }

            network::game::sc::StopRemotePlayer packet;
            packet.id = id.Unwrap();
            GamePacketBuilder::Build(packet.position, *snapshot);

            view.Broadcast(packet, oldSector, id);

            return;
        }

        oldSector.RemoveEntity(id);

        if (GameSpatialSector::Subset outs = (oldSector - newSector);
            !outs.Empty() && outs.HasEntitiesAtLeast(GameEntityType::Player, 0))
        {
            network::game::sc::RemoveRemotePlayer packet;
            GamePacketBuilder::Build(packet, *snapshot);

            view.Broadcast(packet, outs, GameEntityType::Player);
        }

        if (GameSpatialSector::Subset unchanged = (newSector & oldSector);
            !unchanged.Empty() && unchanged.HasEntitiesAtLeast(GameEntityType::Player, 1))
        {
            network::game::sc::StopRemotePlayer packet;
            packet.id = id.Unwrap();
            GamePacketBuilder::Build(packet.position, *snapshot);

            view.Broadcast(packet, unchanged, GameEntityType::Player, id);
        }

        if (GameSpatialSector::Subset ins = (newSector - oldSector);
            !ins.Empty() && ins.HasEntitiesAtLeast(GameEntityType::Player, 0))
        {
            network::game::sc::AddRemotePlayer packet;
            GamePacketBuilder::Build(packet, *snapshot);

            view.Broadcast(packet, ins, GameEntityType::Player);
        }

        newSector.AddEntity(id);

        ZEROSUGAR_LOG_DEBUG(_gameInstance.GetServiceLocator(),
            std::format("movement: old[{}, {}] new[{}, [{}], old_sector[{}, {}], new_sector[{}, {}]",
                oldPosition.x(), oldPosition.y(),
                position.x(), position.y(),
                oldSector.GetId().GetX(), oldSector.GetId().GetY(),
                newSector.GetId().GetX(), newSector.GetId().GetY()));
    }

    void GameSnapshotController::ProcessSprint(game_entity_id_type id)
    {
        GameSnapshotModelContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(id);
        assert(snapshot);

        GameSpatialSector& sector = spatialContainer.GetSector(
            snapshot->GetPosition().x(), snapshot->GetPosition().y());

        network::game::sc::SprintRemotePlayer packet;
        packet.id = id.Unwrap();

        view.Broadcast(packet, sector, id);
    }

    void GameSnapshotController::ProcessRollDodge(game_entity_id_type id, const Eigen::Vector3d& rotation)
    {
        GameSnapshotModelContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(id);
        assert(snapshot);

        GameSpatialSector& sector = spatialContainer.GetSector(
            snapshot->GetPosition().x(), snapshot->GetPosition().y());

        network::game::sc::RollDodgeRemotePlayer packet;
        packet.id = id.Unwrap();
        packet.rotation.pitch = static_cast<float>(rotation.x());
        packet.rotation.yaw = static_cast<float>(rotation.y());
        packet.rotation.roll = static_cast<float>(rotation.z());

        view.Broadcast(packet, sector, id);
    }

    void GameSnapshotController::ProcessPlayerEquipItemChange(game_entity_id_type id, data::EquipPosition pos, const InventoryItem* item)
    {
        assert(IsValid(pos));
        assert(!item || (item && item->slotType == InventoryItemSlotType::Equipment));

        GameSnapshotModelContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(id);
        assert(snapshot);

        snapshot->SetEquipment(pos, item ? *item : std::optional<InventoryItem>());

        network::game::sc::ChangeRemotePlayerEquipItem packet;
        packet.id = id.Unwrap();
        packet.equipPosition = static_cast<int32_t>(pos);
        packet.itemId = item ? item->itemDataId : -1;

        view.Broadcast(packet, id);
    }
}
