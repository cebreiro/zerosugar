#include "game_snapshot_controller.h"

#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/controller/game_entity_controller_interface.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
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

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(entity.GetId());
        assert(snapshot);

        GameSpatialSector& sector = spatialContainer.GetSector(snapshot->GetPosition().x(), snapshot->GetPosition().y());
        {
            network::game::sc::EnterGame packet;
            GamePacketBuilder::Build(packet, _gameInstance, entity, sector);

            Send(packet, snapshot->GetController());
        }

        if (!sector.Empty() && sector.HasEntitiesAtLeast(GameEntityType::Player, 0))
        {
            network::game::sc::AddRemotePlayer packet;
            GamePacketBuilder::Build(packet, *snapshot);

            Broadcast(packet, sector, GameEntityType::Player);
        }

        sector.AddEntity(snapshot->GetId());
    }

    void GameSnapshotController::ProcessMovement(game_entity_id_type id, const Eigen::Vector3d& position)
    {
        GameSnapshotModelContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();

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

            Broadcast(packet, oldSector, id);

            return;
        }

        oldSector.RemoveEntity(id);

        if (GameSpatialSector::Subset outs = (oldSector - newSector);
            !outs.Empty() && outs.HasEntitiesAtLeast(GameEntityType::Player, 0))
        {
            network::game::sc::RemoveRemotePlayer packet;
            GamePacketBuilder::Build(packet, *snapshot);

            Broadcast(packet, outs, GameEntityType::Player);
        }

        if (GameSpatialSector::Subset unchanged = (newSector & oldSector);
            !unchanged.Empty() && unchanged.HasEntitiesAtLeast(GameEntityType::Player, 1))
        {
            network::game::sc::MoveRemotePlayer packet;
            GamePacketBuilder::Build(packet, *snapshot);

            Broadcast(packet, unchanged, GameEntityType::Player, id);
        }

        if (GameSpatialSector::Subset ins = (newSector - oldSector);
            !ins.Empty() && ins.HasEntitiesAtLeast(GameEntityType::Player, 0))
        {
            network::game::sc::AddRemotePlayer packet;
            GamePacketBuilder::Build(packet, *snapshot);

            Broadcast(packet, ins, GameEntityType::Player);
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

            Broadcast(packet, oldSector, id);

            return;
        }

        oldSector.RemoveEntity(id);

        if (GameSpatialSector::Subset outs = (oldSector - newSector);
            !outs.Empty() && outs.HasEntitiesAtLeast(GameEntityType::Player, 0))
        {
            network::game::sc::RemoveRemotePlayer packet;
            GamePacketBuilder::Build(packet, *snapshot);

            Broadcast(packet, outs, GameEntityType::Player);
        }

        if (GameSpatialSector::Subset unchanged = (newSector & oldSector);
            !unchanged.Empty() && unchanged.HasEntitiesAtLeast(GameEntityType::Player, 1))
        {
            network::game::sc::StopRemotePlayer packet;
            packet.id = id.Unwrap();
            GamePacketBuilder::Build(packet.position, *snapshot);

            Broadcast(packet, unchanged, GameEntityType::Player, id);
        }

        if (GameSpatialSector::Subset ins = (newSector - oldSector);
            !ins.Empty() && ins.HasEntitiesAtLeast(GameEntityType::Player, 0))
        {
            network::game::sc::AddRemotePlayer packet;
            GamePacketBuilder::Build(packet, *snapshot);

            Broadcast(packet, ins, GameEntityType::Player);
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

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(id);
        assert(snapshot);

        GameSpatialSector& sector = spatialContainer.GetSector(
            snapshot->GetPosition().x(), snapshot->GetPosition().y());

        network::game::sc::SprintRemotePlayer packet;
        packet.id = id.Unwrap();

        Broadcast(packet, sector, id);
    }

    void GameSnapshotController::ProcessRollDodge(game_entity_id_type id, const Eigen::Vector3d& rotation)
    {
        GameSnapshotModelContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(id);
        assert(snapshot);

        GameSpatialSector& sector = spatialContainer.GetSector(
            snapshot->GetPosition().x(), snapshot->GetPosition().y());

        network::game::sc::RollDodgeRemotePlayer packet;
        packet.id = id.Unwrap();
        packet.rotation.pitch = static_cast<float>(rotation.x());
        packet.rotation.yaw = static_cast<float>(rotation.y());
        packet.rotation.roll = static_cast<float>(rotation.z());

        Broadcast(packet, sector, id);
    }

    void GameSnapshotController::Broadcast(const IPacket& packet, std::optional<game_entity_id_type> excluded)
    {
        for (const std::unique_ptr<GamePlayerSnapshot>& snapshot : _gameInstance.GetSnapshotContainer().GetPlayerRange())
        {
            if (excluded.has_value() && *excluded == snapshot->GetId())
            {
                continue;;
            }

            Send(packet, snapshot->GetController());
        }
    }

    void GameSnapshotController::Broadcast(const IPacket& packet, const GamePlayerSnapshot& middle, std::optional<game_entity_id_type> excluded)
    {
        GameSpatialSector& sector = _gameInstance.GetSpatialContainer().GetSector(middle.GetPosition().x(), middle.GetPosition().y());

        Broadcast(packet, sector, excluded);
    }

    void GameSnapshotController::Broadcast(const IPacket& packet, const detail::game::GameSpatialSet& set, std::optional<game_entity_id_type> excluded)
    {
        GameSnapshotModelContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();

        for (const game_entity_id_type id : set.GetEntities())
        {
            if (excluded.has_value() && *excluded == id)
            {
                continue;
            }

            GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(id);
            assert(snapshot);

            Send(packet, snapshot->GetController());
        }
    }

    void GameSnapshotController::Broadcast(const IPacket& packet, const detail::game::GameSpatialSet& set, GameEntityType type, std::optional<game_entity_id_type> excluded)
    {
        GameSnapshotModelContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();

        for (const game_entity_id_type id : set.GetEntities(type))
        {
            if (excluded.has_value() && *excluded == id)
            {
                continue;
            }

            GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(id);
            assert(snapshot);

            Send(packet, snapshot->GetController());
        }
    }

    void GameSnapshotController::Send(const IPacket& packet, IGameController& controller)
    {
        if (controller.IsSubscriberOf(packet.GetOpcode()))
        {
            controller.Notify(packet);
        }
    }
}
