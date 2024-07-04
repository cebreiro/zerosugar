#include "game_snapshot_controller.h"

#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_view.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_spawner_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/grid/game_spatial_sector.h"
#include "zerosugar/xr/server/game/packet/packet_builder.h"

namespace zerosugar::xr
{
    using namespace network::game;

    GameSnapshotController::GameSnapshotController(GameInstance& instance)
        : _gameInstance(instance)
    {
    }

    GameSnapshotController::~GameSnapshotController()
    {
    }

    void GameSnapshotController::ProcessPlayerSpawn(const GameEntity& entity)
    {
        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(entity.GetId());
        assert(snapshot);

        sc::EnterGame packet;
        GamePacketBuilder::Build(packet, entity, _gameInstance.GetZoneId());

        view.Send(packet, snapshot->GetController());
    }

    void GameSnapshotController::ProcessPlayerActivate(game_entity_id_type playerId)
    {
        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GamePlayerSnapshot* player = snapshotContainer.FindPlayer(playerId);
        assert(player);

        sc::NotifyPlayerActivated activated;
        view.Send(activated, player->GetController());

        GameSpatialSector& sector = spatialContainer.GetSector(player->GetPosition().x(), player->GetPosition().y());
        if (!sector.Empty())
        {
            if (auto range = sector.GetEntities(); range.begin() != range.end())
            {
                sc::AddRemotePlayer addPlayer;
                GamePacketBuilder::Build(addPlayer, *player);

                sc::AddRemotePlayer newPlayers;
                sc::AddMonster newMonsters;

                for (const game_entity_id_type id : range)
                {
                    if (id.GetType() == GameEntityType::Player)
                    {
                        const GamePlayerSnapshot* newPlayer = snapshotContainer.FindPlayer(id);
                        assert(newPlayer);

                        GamePacketBuilder::Build(newPlayers.players.emplace_back(), *newPlayer);
                    }
                    else if (id.GetType() == GameEntityType::Monster)
                    {
                        const GameMonsterSnapshot* newMonster = snapshotContainer.FindMonster(id);
                        assert(newMonster);

                        GamePacketBuilder::Build(newMonsters.monsters.emplace_back(), *newMonster);
                    }

                    IGameController* controller = snapshotContainer.FindController(id);
                    assert(controller);

                    view.Send(addPlayer, *controller);
                }

                newPlayers.playersCount = static_cast<int32_t>(newPlayers.players.size());
                newMonsters.monstersCount = static_cast<int32_t>(newMonsters.monsters.size());

                if (newPlayers.playersCount > 0)
                {
                    view.Send(newPlayers, player->GetController());
                }

                if (newMonsters.monstersCount > 0)
                {
                    view.Send(newMonsters, player->GetController());
                }
            }
        }

        sector.AddEntity(player->GetId());
    }

    void GameSnapshotController::ProcessPlayerDespawn(game_entity_id_type entityId)
    {
        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(entityId);
        assert(snapshot);

        GameSpatialSector& sector = spatialContainer.GetSector(snapshot->GetPosition().x(), snapshot->GetPosition().y());
        sector.RemoveEntity(entityId);

        if (!sector.Empty())
        {
            sc::RemoveRemotePlayer packet;
            GamePacketBuilder::Build(packet, *snapshot);

            view.Broadcast(packet, sector);
        }

        [[maybe_unused]]
        const bool removed = snapshotContainer.Remove(entityId);
        assert(removed);
    }

    void GameSnapshotController::ProcessMovement(game_entity_id_type playerId, const Eigen::Vector3d& position)
    {
        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(playerId);
        if (!snapshot)
        {
            return;
        }

        const Eigen::Vector3d oldPosition = snapshot->GetPosition();
        HandlePlayerPositionChange(*snapshot, oldPosition, position);
    }

    void GameSnapshotController::ProcessStop(game_entity_id_type playerId, const Eigen::Vector3d& position)
    {
        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(playerId);
        if (!snapshot)
        {
            return;
        }

        const Eigen::Vector3d oldPosition = snapshot->GetPosition();
        HandlePlayerPositionChange(*snapshot, oldPosition, position);
    }

    void GameSnapshotController::ProcessSprint(game_entity_id_type id)
    {
        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(id);
        assert(snapshot);

        GameSpatialSector& sector = spatialContainer.GetSector(snapshot->GetPosition().x(), snapshot->GetPosition().y());

        sc::SprintRemotePlayer packet;
        packet.id = id.Unwrap();

        view.Broadcast(packet, sector, id);
    }

    void GameSnapshotController::ProcessRollDodge(game_entity_id_type id, const Eigen::Vector3d& rotation)
    {
        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(id);
        assert(snapshot);

        GameSpatialSector& sector = spatialContainer.GetSector(snapshot->GetPosition().x(), snapshot->GetPosition().y());

        sc::RollDodgeRemotePlayer packet;
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

        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(id);
        assert(snapshot);

        snapshot->SetEquipment(pos, item ? *item : std::optional<InventoryItem>());

        sc::ChangeRemotePlayerEquipItem packet;
        packet.id = id.Unwrap();
        packet.equipPosition = static_cast<int32_t>(pos);
        packet.itemId = item ? item->itemDataId : -1;

        view.Broadcast(packet, id);
    }

    void GameSnapshotController::ProcessMonsterSpawn(const GameMonsterSnapshot& snapshot)
    {
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GameSpatialSector& sector = spatialContainer.GetSector(snapshot.GetPosition().x(), snapshot.GetPosition().y());

        if (!sector.Empty())
        {
            sc::AddMonster packet;
            GamePacketBuilder::Build(packet.monsters.emplace_back(), snapshot);
            packet.monstersCount = 1;

            view.Broadcast(packet, sector);
        }

        sector.AddEntity(snapshot.GetId());
    }

    void GameSnapshotController::ProcessSpawnerAdd(const GameSpawnerSnapshot& snapshot)
    {
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSpatialSector& sector = spatialContainer.GetSector(snapshot.GetPosition().x(), snapshot.GetPosition().y());

        sector.AddEntity(snapshot.GetId());
    }

    void GameSnapshotController::HandlePlayerPositionChange(GamePlayerSnapshot& player, const Eigen::Vector3d& oldPos, const Eigen::Vector3d& newPos)
    {
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();
        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();

        GameSpatialSector& oldSector = spatialContainer.GetSector(oldPos.x(), oldPos.y());
        GameSpatialSector& newSector = spatialContainer.GetSector(newPos.x(), newPos.y());

        if (oldSector.GetId() == newSector.GetId())
        {
            sc::MoveRemotePlayer packet;
            GamePacketBuilder::Build(packet, player);

            view.Broadcast(packet, oldSector, player.GetId());

            return;
        }

        oldSector.RemoveEntity(player.GetId());

        if (GameSpatialSector::Subset outs = (oldSector - newSector); !outs.Empty())
        {
            if (auto range = outs.GetEntities(); range.begin() != range.end())
            {
                sc::RemoveRemotePlayer removePlayer;
                GamePacketBuilder::Build(removePlayer, player);

                sc::RemoveRemotePlayer oldPlayers;
                sc::RemoveMonster oldMonsters;

                for (const game_entity_id_type id : range)
                {
                    if (id.GetType() == GameEntityType::Player)
                    {
                        oldPlayers.players.push_back(id.Unwrap());
                    }
                    else if (id.GetType() == GameEntityType::Monster)
                    {
                        oldMonsters.monsters.push_back(id.Unwrap());
                    }

                    IGameController* controller = snapshotContainer.FindController(id);
                    assert(controller);

                    view.Send(removePlayer, *controller);
                }

                oldPlayers.playersCount = static_cast<int32_t>(oldPlayers.players.size());
                oldMonsters.monstersCount = static_cast<int32_t>(oldMonsters.monsters.size());

                if (oldPlayers.playersCount > 0)
                {
                    view.Send(oldPlayers, player.GetController());
                }

                if (oldMonsters.monstersCount > 0)
                {
                    view.Send(oldMonsters, player.GetController());
                }
            }
        }

        if (GameSpatialSector::Subset unchanged = (newSector & oldSector); !unchanged.Empty())
        {
            sc::MoveRemotePlayer packet;
            GamePacketBuilder::Build(packet, player);

            view.Broadcast(packet, unchanged, player.GetId());
        }

        if (GameSpatialSector::Subset ins = (newSector - oldSector); !ins.Empty())
        {
            if (auto range = ins.GetEntities(); range.begin() != range.end())
            {
                sc::AddRemotePlayer addPlayer;
                GamePacketBuilder::Build(addPlayer, player);

                sc::AddRemotePlayer newPlayers;
                sc::AddMonster newMonsters;

                for (const game_entity_id_type id : range)
                {
                    if (id.GetType() == GameEntityType::Player)
                    {
                        const GamePlayerSnapshot* newPlayer = snapshotContainer.FindPlayer(id);
                        assert(newPlayer);

                        GamePacketBuilder::Build(newPlayers.players.emplace_back(), *newPlayer);
                    }
                    else if (id.GetType() == GameEntityType::Monster)
                    {
                        const GameMonsterSnapshot* newMonster = snapshotContainer.FindMonster(id);
                        assert(newMonster);

                        GamePacketBuilder::Build(newMonsters.monsters.emplace_back(), *newMonster);
                    }

                    IGameController* controller = snapshotContainer.FindController(id);
                    assert(controller);

                    view.Send(addPlayer, *controller);
                }

                newPlayers.playersCount = static_cast<int32_t>(newPlayers.players.size());
                newMonsters.monstersCount = static_cast<int32_t>(newMonsters.monsters.size());

                if (newPlayers.playersCount > 0)
                {
                    view.Send(newPlayers, player.GetController());
                }

                if (newMonsters.monstersCount > 0)
                {
                    view.Send(newMonsters, player.GetController());
                }
            }
        }

        player.SetPosition(newPos);
        newSector.AddEntity(player.GetId());

        ZEROSUGAR_LOG_DEBUG(_gameInstance.GetServiceLocator(),
            std::format("movement: old[{}, {}] new[{}, [{}], old_sector[{}, {}], new_sector[{}, {}]",
                oldPos.x(), oldPos.y(), newPos.x(), newPos.y(),
                oldSector.GetId().GetX(), oldSector.GetId().GetY(),
                newSector.GetId().GetX(), newSector.GetId().GetY()));
    }

    void GameSnapshotController::HandleMonsterPositionChange(GameMonsterSnapshot& monster,
        const Eigen::Vector3d& oldPos, const Eigen::Vector3d& newPos)
    {
        (void)monster;
        (void)oldPos;
        (void)newPos;
    }
}
