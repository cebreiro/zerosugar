#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class GamePlayerSnapshot;
}

namespace zerosugar::xr
{
    class GameSnapshotModelContainer
    {
    public:
        using container_type = boost::unordered::unordered_flat_map<game_entity_id_type, UniquePtrNotNull<GamePlayerSnapshot>>;

    public:
        GameSnapshotModelContainer(const GameSnapshotModelContainer& other) = delete;
        GameSnapshotModelContainer(GameSnapshotModelContainer&& other) noexcept = delete;
        GameSnapshotModelContainer& operator=(const GameSnapshotModelContainer& other) = delete;
        GameSnapshotModelContainer& operator=(GameSnapshotModelContainer&& other) noexcept = delete;

    public:
        GameSnapshotModelContainer() = default;
        ~GameSnapshotModelContainer();

        bool Has(game_entity_id_type id) const;

        bool Add(UniquePtrNotNull<GamePlayerSnapshot> player);
        bool RemovePlayer(game_entity_id_type id);

        auto FindPlayer(game_entity_id_type id) -> GamePlayerSnapshot*;
        auto FindPlayer(game_entity_id_type id) const -> const GamePlayerSnapshot*;

        auto GetPlayerRange() const -> std::ranges::values_view<std::ranges::ref_view<const container_type>>;

    private:
        container_type _playerViews;
    };
}
