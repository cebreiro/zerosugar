#include "game_snapshot_container.h"

#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"

namespace zerosugar::xr
{
    GameSnapshotModelContainer::~GameSnapshotModelContainer()
    {
    }

    bool GameSnapshotModelContainer::Has(game_entity_id_type id) const
    {
        return _playerViews.contains(id);
    }

    bool GameSnapshotModelContainer::Add(UniquePtrNotNull<GamePlayerSnapshot> playerView)
    {
        const game_entity_id_type id = playerView->GetId();

        return _playerViews.try_emplace(id, std::move(playerView)).second;
    }

    bool GameSnapshotModelContainer::RemovePlayer(game_entity_id_type id)
    {
        return _playerViews.erase(id);
    }

    auto GameSnapshotModelContainer::FindPlayer(game_entity_id_type id) -> GamePlayerSnapshot*
    {
        const auto iter = _playerViews.find(id);

        return iter != _playerViews.end() ? iter->second.get() : nullptr;
    }

    auto GameSnapshotModelContainer::FindPlayer(game_entity_id_type id) const -> const GamePlayerSnapshot*
    {
        const auto iter = _playerViews.find(id);

        return iter != _playerViews.end() ? iter->second.get() : nullptr;
    }

    auto GameSnapshotModelContainer::GetPlayerRange() const -> std::ranges::values_view<std::ranges::ref_view<const container_type>>
    {
        return _playerViews | std::views::values;
    }
}
