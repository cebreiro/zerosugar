#pragma once
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_id.h"

namespace zerosugar::xr
{
    class GameSpatialCell
    {
    public:
        GameSpatialCell() = default;
        explicit GameSpatialCell(game_spatial_cell_id_type id);

        bool HasEntity(game_entity_id_type id) const;

        void AddEntity(game_entity_id_type id);
        void RemoveEntity(game_entity_id_type id);

        auto GetId() const -> game_spatial_cell_id_type;
        auto GetEntities() const -> std::span<const game_entity_id_type>;

    private:
        game_spatial_cell_id_type _id;
        std::vector<game_entity_id_type> _entities;
    };

}
