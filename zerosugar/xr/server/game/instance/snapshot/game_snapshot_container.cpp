#include "game_snapshot_container.h"

#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_spawner_snapshot.h"

namespace zerosugar::xr
{
    GameSnapshotContainer::~GameSnapshotContainer()
    {
    }

    bool GameSnapshotContainer::Has(game_entity_id_type id) const
    {
        switch (id.GetType())
        {
        case GameEntityType::Player:
            return _players.contains(id);
        case GameEntityType::Monster:
            return _monsters.contains(id);
        case GameEntityType::Spawner:
            break;
        default:;
        }

        assert(false);

        return false;
    }

    bool GameSnapshotContainer::Add(UniquePtrNotNull<GamePlayerSnapshot> player)
    {
        const game_entity_id_type id = player->GetId();
        assert(id.GetType() == GameEntityType::Player);

        return _players.try_emplace(id, std::move(player)).second;
    }

    bool GameSnapshotContainer::Add(UniquePtrNotNull<GameMonsterSnapshot> monster)
    {
        const game_entity_id_type id = monster->GetId();
        assert(id.GetType() == GameEntityType::Monster);

        return _monsters.try_emplace(id, std::move(monster)).second;
    }

    bool GameSnapshotContainer::Add(UniquePtrNotNull<GameSpawnerSnapshot> spawner)
    {
        const game_entity_id_type id = spawner->GetId();
        assert(id.GetType() == GameEntityType::Spawner);

        return _spawners.try_emplace(id, std::move(spawner)).second;
    }

    bool GameSnapshotContainer::Remove(game_entity_id_type id)
    {
        switch (id.GetType())
        {
        case GameEntityType::Player:
            return _players.erase(id);
        case GameEntityType::Monster:
            return _monsters.erase(id);
        case GameEntityType::Spawner:
            return _spawners.erase(id);
        default:;
        }

        assert(false);

        return false;
    }

    auto GameSnapshotContainer::FindController(game_entity_id_type id) -> IGameController*
    {
        switch (id.GetType())
        {
        case GameEntityType::Player:
        {
            if (const GamePlayerSnapshot* snapshot = FindPlayer(id); snapshot)
            {
                return &snapshot->GetController();
            }

            return nullptr;
        }
        case GameEntityType::Monster:
        {
            if (const GameMonsterSnapshot* snapshot = FindMonster(id); snapshot)
            {
                return &snapshot->GetController();
            }

            return nullptr;
        }
        case GameEntityType::Spawner:
        {
            if (const auto iter = _spawners.find(id); iter != _spawners.end())
            {
                return &iter->second->GetController();
            }

            return nullptr;
        }
        default:;
        }

        assert(false);

        return nullptr;
    }

    auto GameSnapshotContainer::FindPosition(game_entity_id_type id) const -> std::optional<Eigen::Vector3d>
    {
        switch (id.GetType())
        {
        case GameEntityType::Player:
        {
            if (const GamePlayerSnapshot* snapshot = FindPlayer(id); snapshot)
            {
                return snapshot->GetPosition();
            }

            return std::nullopt;
        }
        case GameEntityType::Monster:
        {
            if (const GameMonsterSnapshot* snapshot = FindMonster(id); snapshot)
            {
                return snapshot->GetPosition();
            }

            return std::nullopt;
        }
        case GameEntityType::Spawner:
        {
            if (const auto iter = _spawners.find(id); iter != _spawners.end())
            {
                return iter->second->GetPosition();
            }

            return std::nullopt;
        }
        default:;
        }

        assert(false);

        return std::nullopt;
    }

    auto GameSnapshotContainer::FindPlayer(game_entity_id_type id) -> GamePlayerSnapshot*
    {
        assert(id.GetType() == GameEntityType::Player);

        const auto iter = _players.find(id);

        return iter != _players.end() ? iter->second.get() : nullptr;
    }

    auto GameSnapshotContainer::FindPlayer(game_entity_id_type id) const -> const GamePlayerSnapshot*
    {
        assert(id.GetType() == GameEntityType::Player);
        const auto iter = _players.find(id);

        return iter != _players.end() ? iter->second.get() : nullptr;
    }

    auto GameSnapshotContainer::FindMonster(game_entity_id_type id) -> GameMonsterSnapshot*
    {
        assert(id.GetType() == GameEntityType::Monster);

        const auto iter = _monsters.find(id);

        return iter != _monsters.end() ? iter->second.get() : nullptr;
    }

    auto GameSnapshotContainer::FindMonster(game_entity_id_type id) const -> const GameMonsterSnapshot*
    {
        assert(id.GetType() == GameEntityType::Monster);

        const auto iter = _monsters.find(id);

        return iter != _monsters.end() ? iter->second.get() : nullptr;
    }

    auto GameSnapshotContainer::GetPlayerRange() const -> std::ranges::values_view<std::ranges::ref_view<const player_container_type>>
    {
        return _players | std::views::values;
    }

    auto GameSnapshotContainer::GetMonsterRange() const -> std::ranges::values_view<std::ranges::ref_view<const monster_container_type>>
    {
        return _monsters | std::views::values;
    }
}
