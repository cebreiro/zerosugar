#include "game_snapshot_controller.h"

#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_view.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_spawner_snapshot.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_sector.h"
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

        view.Sync(snapshot->GetController(), packet);
    }

    void GameSnapshotController::ProcessPlayerActivate(game_entity_id_type playerId)
    {
        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GamePlayerSnapshot* player = snapshotContainer.FindPlayer(playerId);
        assert(player);

        sc::NotifyPlayerActivated activated;
        view.Sync(player->GetController(), activated);

        GameSpatialSector& sector = spatialContainer.GetSector(player->GetPosition());
        if (!sector.Empty() && sector.HasEntitiesAtLeast(0))
        {
            sc::AddRemotePlayer addPlayer;
            GamePacketBuilder::Build(addPlayer, *player);

            sc::AddRemotePlayer newPlayers;
            sc::AddMonster newMonsters;

            for (const game_entity_id_type id : sector.GetEntities())
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

                view.Sync(*controller, addPlayer);
            }

            newPlayers.playersCount = static_cast<int32_t>(newPlayers.players.size());
            newMonsters.monstersCount = static_cast<int32_t>(newMonsters.monsters.size());

            if (newPlayers.playersCount > 0)
            {
                view.Sync(player->GetController(), newPlayers);
            }

            if (newMonsters.monstersCount > 0)
            {
                view.Sync(player->GetController(), newMonsters);
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

        GameSpatialSector& sector = spatialContainer.GetSector(snapshot->GetPosition());
        sector.RemoveEntity(entityId);

        sc::RemoveRemotePlayer packet;
        GamePacketBuilder::Build(packet, *snapshot);

        view.Broadcast(packet, sector);

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

        constexpr bool syncMovement = true;
        const Eigen::Vector3d oldPosition = snapshot->GetPosition();

        HandlePlayerPositionChange(*snapshot, oldPosition, position, syncMovement);
    }

    void GameSnapshotController::ProcessStop(game_entity_id_type playerId, const Eigen::Vector3d& position)
    {
        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(playerId);
        if (!snapshot)
        {
            return;
        }

        constexpr bool syncMovement = true;
        const Eigen::Vector3d oldPosition = snapshot->GetPosition();

        HandlePlayerPositionChange(*snapshot, oldPosition, position, syncMovement);
    }

    void GameSnapshotController::ProcessSprint(game_entity_id_type id)
    {
        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(id);
        assert(snapshot);

        GameSpatialSector& sector = spatialContainer.GetSector(snapshot->GetPosition());

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

        GameSpatialSector& sector = spatialContainer.GetSector(snapshot->GetPosition());

        sc::RollDodgeRemotePlayer packet;
        packet.id = id.Unwrap();
        packet.rotation.pitch = static_cast<float>(rotation.x());
        packet.rotation.yaw = static_cast<float>(rotation.y());
        packet.rotation.roll = static_cast<float>(rotation.z());

        view.Broadcast(packet, sector, id);
    }

    void GameSnapshotController::ProcessPlayerAttack(game_entity_id_type playerId, int32_t attackId,
        const Eigen::Vector3d& position, const Eigen::Vector3d& rotation)
    {
        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();
        GamePlayerSnapshot* snapshot = snapshotContainer.FindPlayer(playerId);
        assert(snapshot);

        const Eigen::Vector3d oldPos = snapshot->GetPosition();

        if (oldPos != position)
        {
            constexpr bool syncMovement = false;

            HandlePlayerPositionChange(*snapshot, oldPos, position, syncMovement);
        }

        sc::RemotePlayerAttack packet;
        packet.id = playerId.Unwrap();
        packet.motionId = attackId;
        packet.position.x = static_cast<float>(position.x());
        packet.position.y = static_cast<float>(position.y());
        packet.position.z = static_cast<float>(position.z());
        packet.rotation.yaw = static_cast<float>(rotation.y());

        _gameInstance.GetSnapshotView().Broadcast(packet, playerId);
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

        sc::SpawnMonster packet;
        GamePacketBuilder::Build(packet.monsters.emplace_back(), snapshot);
        packet.monstersCount = 1;

        view.Broadcast(packet, sector);

        sector.AddEntity(snapshot.GetId());
    }

    void GameSnapshotController::ProcessMonsterMove(GameMonsterSnapshot& snapshot, const Eigen::Vector3d& position, float yaw)
    {
        snapshot.SetYaw(yaw);
        const Eigen::Vector3d oldPos = snapshot.GetPosition();

        constexpr bool syncMovement = true;

        HandleMonsterPositionChange(snapshot, oldPos, position, syncMovement);
    }

    void GameSnapshotController::ProcessMonsterAttack(GameMonsterSnapshot& snapshot, int32_t attackIndex,
        const std::optional<Eigen::Vector3d>& destPos, const Eigen::Vector3d& rotation, double movementDuration)
    {
        const Eigen::Vector3d& position = snapshot.GetPosition();

        sc::AttackMonster packet;
        packet.id = snapshot.GetId().Unwrap();
        packet.actionIndex = attackIndex;
        packet.position.x = static_cast<float>(position.x());
        packet.position.y = static_cast<float>(position.y());
        packet.position.z = static_cast<float>(position.z());
        packet.rotation.pitch = static_cast<float>(rotation.x());
        packet.rotation.yaw = static_cast<float>(rotation.y());
        packet.rotation.roll = static_cast<float>(rotation.z());

        if (destPos.has_value())
        {
            packet.destPosition.x = static_cast<float>(destPos->x());
            packet.destPosition.y = static_cast<float>(destPos->y());
            packet.destPosition.z = static_cast<float>(destPos->z());
            packet.destMovementDuration = movementDuration;
        }

        GameSnapshotView& view = _gameInstance.GetSnapshotView();
        view.Broadcast(packet, _gameInstance.GetSpatialContainer().GetSector(snapshot.GetPosition()));

        if (destPos.has_value())
        {
            const Eigen::Vector3d oldPos = snapshot.GetPosition();

            constexpr bool syncMovement = false;

            HandleMonsterPositionChange(snapshot, oldPos, *destPos, syncMovement);
        }
    }

    void GameSnapshotController::ProcessMonsterAttackEffect(game_entity_id_type attackerId, int32_t attackId,
        game_entity_id_type targetId, float targetHP)
    {
        sc::BeAttackedPlayer packet;
        packet.attackerId = attackerId.Unwrap();
        packet.monsterActionIndex = attackId;
        packet.attackedId = targetId.Unwrap();
        packet.attackedHp = targetHP;

        const GamePlayerSnapshot* attacked = _gameInstance.GetSnapshotContainer().FindPlayer(targetId);
        assert(attacked);

        _gameInstance.GetSnapshotView().Broadcast(packet,
            _gameInstance.GetSpatialContainer().GetSector(attacked->GetPosition()));
    }

    void GameSnapshotController::ProcessSpawnerAdd(const GameSpawnerSnapshot& snapshot)
    {
        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSpatialSector& sector = spatialContainer.GetSector(snapshot.GetPosition());

        sector.AddEntity(snapshot.GetId());
    }

    void GameSnapshotController::HandlePlayerPositionChange(GamePlayerSnapshot& player, const Eigen::Vector3d& oldPos, const Eigen::Vector3d& newPos, bool syncMovement)
    {
        player.SetPosition(newPos);

        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();
        GameSnapshotContainer& snapshotContainer = _gameInstance.GetSnapshotContainer();

        GameSpatialSector& oldSector = spatialContainer.GetSector(oldPos);
        GameSpatialSector& newSector = spatialContainer.GetSector(newPos);

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

                    view.Sync(*controller, removePlayer);
                }

                oldPlayers.playersCount = static_cast<int32_t>(oldPlayers.players.size());
                oldMonsters.monstersCount = static_cast<int32_t>(oldMonsters.monsters.size());

                if (oldPlayers.playersCount > 0)
                {
                    view.Sync(player.GetController(), oldPlayers);
                }

                if (oldMonsters.monstersCount > 0)
                {
                    view.Sync(player.GetController(), oldMonsters);
                }
            }
        }

        if (syncMovement)
        {
            if (GameSpatialSector::Subset unchanged = (newSector & oldSector); !unchanged.Empty())
            {
                sc::MoveRemotePlayer packet;
                GamePacketBuilder::Build(packet, player);

                view.Broadcast(packet, unchanged, player.GetId());
            }
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

                    view.Sync(*controller, addPlayer);
                }

                newPlayers.playersCount = static_cast<int32_t>(newPlayers.players.size());
                newMonsters.monstersCount = static_cast<int32_t>(newMonsters.monsters.size());

                if (newPlayers.playersCount > 0)
                {
                    view.Sync(player.GetController(), newPlayers);
                }

                if (newMonsters.monstersCount > 0)
                {
                    view.Sync(player.GetController(), newMonsters);
                }
            }
        }

        newSector.AddEntity(player.GetId());

        ZEROSUGAR_LOG_DEBUG(_gameInstance.GetServiceLocator(),
            std::format("player movement: old[{}, {}] new[{}, [{}], old_sector[{}, {}], new_sector[{}, {}]",
                oldPos.x(), oldPos.y(), newPos.x(), newPos.y(),
                oldSector.GetId().GetX(), oldSector.GetId().GetY(),
                newSector.GetId().GetX(), newSector.GetId().GetY()));
    }

    void GameSnapshotController::HandleMonsterPositionChange(GameMonsterSnapshot& monster,
        const Eigen::Vector3d& oldPos, const Eigen::Vector3d& newPos, bool syncMovement)
    {
        monster.SetPosition(newPos);

        GameSpatialContainer& spatialContainer = _gameInstance.GetSpatialContainer();
        GameSnapshotView& view = _gameInstance.GetSnapshotView();

        GameSpatialSector& oldSector = spatialContainer.GetSector(oldPos);
        GameSpatialSector& newSector = spatialContainer.GetSector(newPos);

        if (oldSector.GetId() == newSector.GetId())
        {
            if (syncMovement)
            {
                sc::MoveMonster packet;
                packet.id = monster.GetId().Unwrap();
                packet.position.x = static_cast<float>(monster.GetPosition().x());
                packet.position.y = static_cast<float>(monster.GetPosition().y());
                packet.position.z = static_cast<float>(monster.GetPosition().z());
                packet.rotation.yaw = monster.GetYaw();

                view.Broadcast(packet, oldSector);
            }

            return;
        }

        oldSector.RemoveEntity(monster.GetId());

        if (GameSpatialSector::Subset outs = (oldSector - newSector); !outs.Empty())
        {
            sc::RemoveMonster removeMonster;
            removeMonster.monstersCount = 1;
            removeMonster.monsters.emplace_back(monster.GetId().Unwrap());

            view.Broadcast(GameEntityType::Player, removeMonster, outs);
        }

        if (syncMovement)
        {
            if (GameSpatialSector::Subset unchanged = (newSector & oldSector); !unchanged.Empty())
            {
                sc::MoveMonster packet;
                packet.id = monster.GetId().Unwrap();
                packet.position.x = static_cast<float>(monster.GetPosition().x());
                packet.position.y = static_cast<float>(monster.GetPosition().y());
                packet.position.z = static_cast<float>(monster.GetPosition().z());
                packet.rotation.yaw = monster.GetYaw();

                view.Broadcast(packet, unchanged, monster.GetId());
            }
        }

        if (GameSpatialSector::Subset ins = (newSector - oldSector); !ins.Empty())
        {
            sc::AddMonster addMonster;
            addMonster.monstersCount = 1;

            GamePacketBuilder::Build(addMonster.monsters.emplace_back(), monster);

            view.Broadcast(GameEntityType::Player, addMonster, ins);
        }

        newSector.AddEntity(monster.GetId());

        ZEROSUGAR_LOG_DEBUG(_gameInstance.GetServiceLocator(),
            std::format("monster movement: monster[{}] old[{}, {}] new[{}, [{}], old_sector[{}, {}], new_sector[{}, {}]",
                monster.GetId().Unwrap(),
                oldPos.x(), oldPos.y(), newPos.x(), newPos.y(),
                oldSector.GetId().GetX(), oldSector.GetId().GetY(),
                newSector.GetId().GetX(), newSector.GetId().GetY()));
    }
}
