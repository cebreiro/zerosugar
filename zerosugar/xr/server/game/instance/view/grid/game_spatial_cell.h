#pragma once
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"
#include "zerosugar/xr/server/game/instance/view/grid/game_spatial_id.h"

namespace zerosugar::xr
{
    class GameSpatialCell
    {
    public:
        using container_type = std::array<std::vector<game_entity_id_type>, static_cast<int32_t>(GameEntityType::Count)>;

    public:
        explicit GameSpatialCell(game_spatial_cell_id_type id);

        bool HasEntity(game_entity_id_type id) const;

        void AddEntity(game_entity_id_type id);
        void RemoveEntity(game_entity_id_type id);

        auto GetId() const -> game_spatial_cell_id_type;
        auto GetSize() const -> int64_t;
        auto GetSize(GameEntityType type) const -> int64_t;

        auto GetEntities(GameEntityType type) -> std::span<game_entity_id_type>;
        auto GetEntities(GameEntityType type) const -> std::span<const game_entity_id_type>;

        auto GetEntities() -> std::ranges::join_view<std::ranges::ref_view<container_type>>;
        auto GetEntities() const -> std::ranges::join_view<std::ranges::ref_view<const container_type>>;

    private:
        auto GetContainer(GameEntityType type) -> std::vector<game_entity_id_type>&;
        auto GetContainer(GameEntityType type) const -> const std::vector<game_entity_id_type>&;

    private:
        game_spatial_cell_id_type _id;

        std::array<std::vector<game_entity_id_type>, static_cast<int32_t>(GameEntityType::Count)> _entities;
    };
}