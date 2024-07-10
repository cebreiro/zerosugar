#pragma once
#include <boost/container/static_vector.hpp>
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_id.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_cell.h"

namespace zerosugar::xr
{
    class GameSpatialSet
    {
    public:
        using cell_container_type = boost::container::static_vector<PtrNotNull<GameSpatialCell>, 9>;

    public:
        virtual ~GameSpatialSet() = default;

        bool Empty() const;

        bool HasEntitiesAtLeast(int64_t count) const;
        bool HasEntitiesAtLeast(GameEntityType type, int64_t count) const;
        bool HasCell(game_spatial_cell_id_type id) const;

        virtual void AddCell(PtrNotNull<GameSpatialCell> cell);

        inline auto GetCells() const;
        inline auto GetEntities() const;
        inline auto GetEntities(GameEntityType type) const;

    protected:
        static bool CellCompare(PtrNotNull<const GameSpatialCell> lhs, PtrNotNull<const GameSpatialCell> rhs);

    protected:
        cell_container_type _cells;
    };

    auto GameSpatialSet::GetCells() const
    {
        return _cells
            | std::views::transform([](PtrNotNull<const GameSpatialCell> cell) -> const GameSpatialCell&
                {
                    return *cell;
                });
    }

    auto GameSpatialSet::GetEntities() const
    {
        return _cells
            | std::views::transform([](PtrNotNull<const GameSpatialCell> cell)
                {
                    return cell->GetEntities();
                })
            | std::views::join;
    }

    auto GameSpatialSet::GetEntities(GameEntityType type) const
    {
        return _cells
            | std::views::transform([type](PtrNotNull<const GameSpatialCell> cell)
                {
                    return cell->GetEntities(type);
                })
            | std::views::join;
    }
}
