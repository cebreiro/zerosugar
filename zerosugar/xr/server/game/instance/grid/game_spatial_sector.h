#pragma once
#include <boost/container/static_vector.hpp>
#include "zerosugar/xr/server/game/instance/grid/game_spatial_cell.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_id.h"

namespace zerosugar::xr
{
    class GameSpatialCell;
}

namespace zerosugar::xr
{
    class GameSpatialSector
    {
    public:
        using cell_container_type = boost::container::static_vector<PtrNotNull<GameSpatialCell>, 9>;

        struct Transform
        {
            auto operator()(PtrNotNull<GameSpatialCell> cell) const -> std::span<const game_entity_id_type>;
        };

        using entity_id_view_type = std::ranges::join_view<
            std::ranges::transform_view<
                std::ranges::ref_view<const cell_container_type>, Transform>>;

        class View
        {
        public:
            View() = default;

            void AddEntity(game_entity_id_type id);
            void RemoveEntity(game_entity_id_type id);

            auto GetEntities() const -> entity_id_view_type;

        private:
            friend class GameSpatialSector;

            cell_container_type _cells;
        };

    public:
        GameSpatialSector() = default;
        explicit GameSpatialSector(game_spatial_sector_id_type id);

        bool HasCell(game_spatial_cell_id_type id) const;

        void AddCell(PtrNotNull<GameSpatialCell> cell);
        void AddEntity(game_entity_id_type id);

        auto Difference(const GameSpatialSector& other) const -> View;
        auto Intersect(const GameSpatialSector& other) const -> View;

        auto GetId() const -> game_spatial_sector_id_type;
        auto GetEntities() const -> entity_id_view_type;

    public:
        friend auto operator-(const GameSpatialSector& lhs, const GameSpatialSector& rhs) -> View;

    private:
        game_spatial_sector_id_type _id;
        cell_container_type _cells;
    };
}
