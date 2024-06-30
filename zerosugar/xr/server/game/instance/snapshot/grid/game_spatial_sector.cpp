#include "game_spatial_sector.h"

#include "zerosugar/xr/server/game/instance/snapshot/grid/game_spatial_cell.h"

namespace zerosugar::xr
{
    struct GameSpatialCellCompare
    {
        bool operator()(PtrNotNull<GameSpatialCell> lhs, PtrNotNull<GameSpatialCell> rhs) const
        {
            return lhs->GetId() < rhs->GetId();
        }
    };
}

namespace zerosugar::xr::detail::game
{
    auto CellTransform::operator()(PtrNotNull<GameSpatialCell> cell) const
        -> std::ranges::join_view<std::ranges::ref_view<GameSpatialCell::container_type>>
    {
        return cell->GetEntities();
    }

    CellFilter::CellFilter(GameEntityType type)
        : type(type)
    {
    }

    auto CellFilter::operator()(PtrNotNull<GameSpatialCell> cell) const -> std::span<const game_entity_id_type>
    {
        return cell->GetEntities(type);
    }

    auto GameSpatialSet::GetEntities() const -> entity_id_view_type
    {
        return _cells | std::views::transform(CellTransform()) | std::views::join;
    }

    auto GameSpatialSet::GetEntities(GameEntityType type) const -> entity_id_filter_view_type
    {
        return _cells | std::views::transform(CellFilter(type)) | std::views::join;
    }

    bool GameSpatialSet::Empty() const
    {
        return _cells.empty();
    }

    bool GameSpatialSet::HasEntitiesAtLeast(int64_t count) const
    {
        int64_t sum = 0;

        for (PtrNotNull<GameSpatialCell> cell : _cells)
        {
            sum += cell->GetSize();

            if (sum >= count)
            {
                return true;
            }
        }

        return false;
    }

    bool GameSpatialSet::HasEntitiesAtLeast(GameEntityType type, int64_t count) const
    {
        int64_t sum = 0;

        for (PtrNotNull<GameSpatialCell> cell : _cells)
        {
            sum += cell->GetSize(type);

            if (sum >= count)
            {
                return true;
            }
        }

        return false;
    }

    bool GameSpatialSet::HasCell(game_spatial_cell_id_type id) const
    {
        const auto iter = std::ranges::find_if(_cells, [id](PtrNotNull<GameSpatialCell> cell) -> bool
            {
                return cell->GetId() == id;
            });

        return iter != _cells.end();
    }

    void GameSpatialSet::AddCell(PtrNotNull<GameSpatialCell> cell)
    {
        assert(!HasCell(cell->GetId()));
        assert(_cells.size() + 1 <= _cells.max_size());

        auto iter = std::ranges::lower_bound(_cells, cell, GameSpatialCellCompare{});
        _cells.insert(iter, cell);
    }
}

namespace zerosugar::xr
{
    GameSpatialSector::GameSpatialSector(game_spatial_sector_id_type id)
        : _id(id)
    {
    }

    void GameSpatialSector::AddCell(PtrNotNull<GameSpatialCell> cell)
    {
        GameSpatialSet::AddCell(cell);

        if (_middle)
        {
            return;
        }

        if (const game_spatial_cell_id_type cellID = cell->GetId();
            cellID.GetX() == _id.GetX() && cell->GetId().GetY() == _id.GetY())
        {
            _middle = cell;
        }
    }

    void GameSpatialSector::AddEntity(game_entity_id_type id)
    {
        assert(_middle);

        _middle->AddEntity(id);
    }

    void GameSpatialSector::RemoveEntity(game_entity_id_type id)
    {
        assert(_middle);

        _middle->RemoveEntity(id);
    }

    auto GameSpatialSector::Difference(const GameSpatialSector& other) const -> Subset
    {
        Subset view;
        std::ranges::set_difference(_cells, other._cells,
            std::back_inserter(view._cells), GameSpatialCellCompare());

        return view;
    }

    auto GameSpatialSector::Intersect(const GameSpatialSector& other) const -> Subset
    {
        Subset view;
        std::ranges::set_intersection(_cells, other._cells,
            std::back_inserter(view._cells), GameSpatialCellCompare());

        return view;
    }

    auto GameSpatialSector::GetId() const -> game_spatial_sector_id_type
    {
        return _id;
    }

    auto operator-(const GameSpatialSector& lhs, const GameSpatialSector& rhs) -> GameSpatialSector::Subset
    {
        return lhs.Difference(rhs);
    }

    GameSpatialSector::Subset operator&(const GameSpatialSector& lhs, const GameSpatialSector& rhs)
    {
        return lhs.Intersect(rhs);
    }
}
