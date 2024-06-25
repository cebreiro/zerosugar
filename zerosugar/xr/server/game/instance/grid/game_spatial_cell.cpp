#include "game_spatial_cell.h"

namespace zerosugar::xr
{
    GameSpatialCell::GameSpatialCell(game_spatial_cell_id_type id)
        : _id(id)
    {
    }

    bool GameSpatialCell::HasEntity(game_entity_id_type id) const
    {
        const auto iter = std::ranges::find(_entities, id);

        return iter != _entities.end();
    }

    void GameSpatialCell::AddEntity(game_entity_id_type id)
    {
        assert(!HasEntity(id));

        _entities.emplace_back(id);
    }

    void GameSpatialCell::RemoveEntity(game_entity_id_type id)
    {
        const auto iter = std::ranges::find(_entities, id);
        if (iter != _entities.end())
        {
            _entities.erase(iter);
        }
        else
        {
            assert(false);
        }
    }

    auto GameSpatialCell::GetId() const -> game_spatial_cell_id_type
    {
        return _id;
    }

    auto GameSpatialCell::GetEntities() const -> std::span<const game_entity_id_type>
    {
        return { _entities.data(), _entities.size() };
    }
}
