#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class GamePlayerViewModel;
}

namespace zerosugar::xr
{
    class GameViewModelContainer
    {
    public:
        using container_type = boost::unordered::unordered_flat_map<game_entity_id_type, UniquePtrNotNull<GamePlayerViewModel>>;

    public:
        GameViewModelContainer(const GameViewModelContainer& other) = delete;
        GameViewModelContainer(GameViewModelContainer&& other) noexcept = delete;
        GameViewModelContainer& operator=(const GameViewModelContainer& other) = delete;
        GameViewModelContainer& operator=(GameViewModelContainer&& other) noexcept = delete;

    public:
        GameViewModelContainer() = default;
        ~GameViewModelContainer();

        bool Has(game_entity_id_type id) const;

        bool Add(UniquePtrNotNull<GamePlayerViewModel> player);
        bool RemovePlayer(game_entity_id_type id);

        auto FindPlayer(game_entity_id_type id) -> GamePlayerViewModel*;
        auto FindPlayer(game_entity_id_type id) const -> const GamePlayerViewModel*;

        auto GetPlayerRange() const -> std::ranges::values_view<std::ranges::ref_view<const container_type>>;

    private:
        container_type _playerViews;
    };
}
