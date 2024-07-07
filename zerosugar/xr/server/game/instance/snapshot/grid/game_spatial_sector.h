#pragma once
#include <boost/container/static_vector.hpp>
#include "zerosugar/xr/server/game/instance/snapshot/grid/game_spatial_cell.h"
#include "zerosugar/xr/server/game/instance/snapshot/grid/game_spatial_id.h"

namespace zerosugar::xr
{
    namespace detail::game
    {
        struct CellTransform
        {
            auto operator()(PtrNotNull<GameSpatialCell> cell) const
                -> std::ranges::join_view<std::ranges::ref_view<GameSpatialCell::container_type>>;
        };

        struct CellFilter
        {
            CellFilter() = delete;
            explicit CellFilter(GameEntityType type);

            auto operator()(PtrNotNull<GameSpatialCell> cell) const -> std::span<const game_entity_id_type>;

            GameEntityType type;
        };

        class GameSpatialSet
        {
        public:
            using cell_container_type = boost::container::static_vector<PtrNotNull<GameSpatialCell>, 9>;

            using entity_id_view_type = std::ranges::join_view<
                std::ranges::transform_view<
                std::ranges::ref_view<const cell_container_type>, CellTransform>>;

            using entity_id_filter_view_type = std::ranges::join_view<
                std::ranges::transform_view<
                std::ranges::ref_view<const cell_container_type>, CellFilter>>;

        public:
            virtual ~GameSpatialSet() = default;

            bool Empty() const;

            bool HasEntitiesAtLeast(int64_t count) const;
            bool HasEntitiesAtLeast(GameEntityType type, int64_t count) const;
            bool HasCell(game_spatial_cell_id_type id) const;

            virtual void AddCell(PtrNotNull<GameSpatialCell> cell);

            auto GetCells() const
            {
                return _cells | std::views::transform([](PtrNotNull<const GameSpatialCell> cell) -> const GameSpatialCell&
                    {
                        return *cell;
                    });
            }
            auto GetEntities() const -> entity_id_view_type;
            auto GetEntities(GameEntityType type) const -> entity_id_filter_view_type;

        protected:
            cell_container_type _cells;
        };
    }

    class GameSpatialSector final : public detail::game::GameSpatialSet
    {
    public:
        class Subset : public GameSpatialSet
        {
            friend class GameSpatialSector;

        public:
            Subset() = default;
        };

    public:
        explicit GameSpatialSector(game_spatial_sector_id_type id);

        void AddCell(PtrNotNull<GameSpatialCell> cell) override;

        void AddEntity(game_entity_id_type id);
        void RemoveEntity(game_entity_id_type id);

        auto Difference(const GameSpatialSector& other) const -> Subset;
        auto Intersect(const GameSpatialSector& other) const -> Subset;

        auto GetId() const -> game_spatial_sector_id_type;
        auto GetCenter() const -> const GameSpatialCell&;
        inline auto GetPeripherals() const;

    public:
        friend auto operator-(const GameSpatialSector& lhs, const GameSpatialSector& rhs) -> Subset;
        friend auto operator&(const GameSpatialSector& lhs, const GameSpatialSector& rhs) -> Subset;

    private:
        game_spatial_sector_id_type _id;

        PtrNotNull<GameSpatialCell> _center = nullptr;
    };

    auto GameSpatialSector::GetPeripherals() const
    {
        return _cells
            | std::views::filter([this](PtrNotNull<const GameSpatialCell> cell)
                {
                    return cell != _center;
                })
            | std::views::transform([](PtrNotNull<const GameSpatialCell> cell) -> const GameSpatialCell&
                {
                    return *cell;
                });
    }
}
