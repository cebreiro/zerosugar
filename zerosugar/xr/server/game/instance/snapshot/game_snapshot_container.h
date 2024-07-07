#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class IGameController;
    class GamePlayerSnapshot;
    class GameMonsterSnapshot;
    class GameSpawnerSnapshot;
}

namespace zerosugar::xr
{
    class GameSnapshotContainer
    {
    public:
        using player_container_type = boost::unordered::unordered_flat_map<game_entity_id_type, UniquePtrNotNull<GamePlayerSnapshot>>;
        using monster_container_type = boost::unordered::unordered_flat_map<game_entity_id_type, UniquePtrNotNull<GameMonsterSnapshot>>;
        using spawner_container_type = boost::unordered::unordered_flat_map<game_entity_id_type, UniquePtrNotNull<GameSpawnerSnapshot>>;

    public:
        GameSnapshotContainer(const GameSnapshotContainer& other) = delete;
        GameSnapshotContainer(GameSnapshotContainer&& other) noexcept = delete;
        GameSnapshotContainer& operator=(const GameSnapshotContainer& other) = delete;
        GameSnapshotContainer& operator=(GameSnapshotContainer&& other) noexcept = delete;

    public:
        GameSnapshotContainer() = default;
        ~GameSnapshotContainer();

        bool Has(game_entity_id_type id) const;

        bool Add(UniquePtrNotNull<GamePlayerSnapshot> player);
        bool Add(UniquePtrNotNull<GameMonsterSnapshot> monster);
        bool Add(UniquePtrNotNull<GameSpawnerSnapshot> spawner);

        bool Remove(game_entity_id_type id);

        auto FindController(game_entity_id_type id) -> IGameController*;

        auto FindPlayer(game_entity_id_type id) -> GamePlayerSnapshot*;
        auto FindPlayer(game_entity_id_type id) const -> const GamePlayerSnapshot*;

        auto FindMonster(game_entity_id_type id) -> GameMonsterSnapshot*;
        auto FindMonster(game_entity_id_type id) const -> const GameMonsterSnapshot*;

        auto GetPlayerRange() const -> std::ranges::values_view<std::ranges::ref_view<const player_container_type>>;
        auto GetMonsterRange() const ->std::ranges::values_view<std::ranges::ref_view<const monster_container_type>>;

    private:
        player_container_type _players;
        monster_container_type _monsters;
        spawner_container_type _spawners;
    };
}
