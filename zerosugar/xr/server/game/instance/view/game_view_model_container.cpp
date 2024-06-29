#include "game_view_model_container.h"

#include "zerosugar/xr/server/game/instance/view/game_player_view_model.h"

namespace zerosugar::xr
{
    GameViewModelContainer::~GameViewModelContainer()
    {
    }

    bool GameViewModelContainer::Has(game_entity_id_type id) const
    {
        return _playerViews.contains(id);
    }

    bool GameViewModelContainer::Add(UniquePtrNotNull<GamePlayerViewModel> playerView)
    {
        const game_entity_id_type id = playerView->GetId();

        return _playerViews.try_emplace(id, std::move(playerView)).second;
    }

    bool GameViewModelContainer::RemovePlayer(game_entity_id_type id)
    {
        return _playerViews.erase(id);
    }

    auto GameViewModelContainer::FindPlayer(game_entity_id_type id) -> GamePlayerViewModel*
    {
        const auto iter = _playerViews.find(id);

        return iter != _playerViews.end() ? iter->second.get() : nullptr;
    }

    auto GameViewModelContainer::FindPlayer(game_entity_id_type id) const -> const GamePlayerViewModel*
    {
        const auto iter = _playerViews.find(id);

        return iter != _playerViews.end() ? iter->second.get() : nullptr;
    }

    auto GameViewModelContainer::GetPlayerRange() const -> std::ranges::values_view<std::ranges::ref_view<const container_type>>
    {
        return _playerViews | std::views::values;
    }
}
