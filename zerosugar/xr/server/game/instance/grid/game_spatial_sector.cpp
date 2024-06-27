#include "game_spatial_sector.h"

#include "zerosugar/xr/server/game/instance/grid/game_spatial_cell.h"

namespace zerosugar::xr
{
    struct GameSpatialCellCompare
    {
        bool operator()(PtrNotNull<GameSpatialCell> lhs, PtrNotNull<GameSpatialCell> rhs) const
        {
            return lhs->GetId() < rhs->GetId();
        }
    };

    auto GameSpatialSector::Transform::operator()(PtrNotNull<GameSpatialCell> cell) const -> std::span<const game_entity_id_type>
    {
        return cell->GetEntities();
    }

    GameSpatialSector::GameSpatialSector(game_spatial_sector_id_type id)
        : _id(id)
    {
    }

    bool GameSpatialSector::HasCell(game_spatial_cell_id_type id) const
    {
        const auto iter = std::ranges::find_if(_cells, [id](PtrNotNull<GameSpatialCell> cell) -> bool
            {
                return cell->GetId() == id;
            });

        return iter != _cells.end();
    }

    void GameSpatialSector::AddCell(PtrNotNull<GameSpatialCell> cell)
    {
        assert(!HasCell(cell->GetId()));
        assert(_cells.size() + 1 <= _cells.max_size());

        auto iter = std::ranges::lower_bound(_cells, cell, GameSpatialCellCompare{});
        _cells.insert(iter, cell);
    }

    void GameSpatialSector::AddEntity(game_entity_id_type id)
    {
        for (PtrNotNull<GameSpatialCell> cell : _cells)
        {
            cell->AddEntity(id);
        }
    }

    auto GameSpatialSector::Difference(const GameSpatialSector& other) const -> View
    {
        View view;
        std::ranges::set_difference(_cells, other._cells,
            std::back_inserter(view._cells), GameSpatialCellCompare());

        return view;
    }

    auto GameSpatialSector::Intersect(const GameSpatialSector& other) const -> View
    {
        View view;
        std::ranges::set_intersection(_cells, other._cells,
            std::back_inserter(view._cells), GameSpatialCellCompare());

        return view;
    }

    auto GameSpatialSector::GetId() const -> game_spatial_sector_id_type
    {
        return _id;
    }

    void GameSpatialSector::View::AddEntity(game_entity_id_type id)
    {
        for (PtrNotNull<GameSpatialCell> cell : _cells)
        {
            cell->AddEntity(id);
        }
    }

    void GameSpatialSector::View::RemoveEntity(game_entity_id_type id)
    {
        for (PtrNotNull<GameSpatialCell> cell : _cells)
        {
            cell->RemoveEntity(id);
        }
    }

    auto GameSpatialSector::View::GetEntities() const -> entity_id_view_type
    {
        return _cells | std::views::transform(Transform()) | std::views::join;
    }

    auto GameSpatialSector::GetEntities() const -> entity_id_view_type
    {
        return _cells | std::views::transform(Transform()) | std::views::join;
    }

    auto operator-(const GameSpatialSector& lhs, const GameSpatialSector& rhs) -> GameSpatialSector::View
    {
        return lhs.Difference(rhs);
    }
}
