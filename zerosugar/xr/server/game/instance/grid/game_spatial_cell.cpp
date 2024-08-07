#include "game_spatial_cell.h"

namespace zerosugar::xr
{
    GameSpatialCell::GameSpatialCell(game_spatial_cell_id_type id, const GameSpatialMBR& mbr)
        : _id(id)
        , _mbr(mbr)
        , _halfX((_mbr.GetMin().x() + _mbr.GetMax().x()) / 2.0)
        , _halfY((_mbr.GetMin().y() + _mbr.GetMax().y()) / 2.0)
    {
    }

    bool GameSpatialCell::Empty(GameEntityType type) const
    {
        return GetContainer(type).empty();
    }

    bool GameSpatialCell::HasEntity(GameEntityType type) const
    {
        return !Empty(type);
    }

    bool GameSpatialCell::HasEntity(game_entity_id_type id) const
    {
        auto& contains = GetContainer(id.GetType());

        const auto iter = std::ranges::find(contains, id);

        return iter != contains.end();
    }

    void GameSpatialCell::AddEntity(game_entity_id_type id)
    {
        assert(!HasEntity(id));

        auto& contains = GetContainer(id.GetType());

        contains.emplace_back(id);
    }

    void GameSpatialCell::RemoveEntity(game_entity_id_type id)
    {
        auto& contains = GetContainer(id.GetType());

        const auto iter = std::ranges::find(contains, id);
        if (iter != contains.end())
        {
            contains.erase(iter);
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

    auto GameSpatialCell::GetMBR() const -> const GameSpatialMBR&
    {
        return _mbr;
    }

    auto GameSpatialCell::GetQuadIndex(const Eigen::Vector2d& position) const -> int64_t
    {
        assert(position.x() >= _mbr.GetMin().x() && position.x() <= _mbr.GetMax().x());
        assert(position.y() >= _mbr.GetMin().y() && position.y() <= _mbr.GetMax().y());

        // quad index
        // ----------- X
        // | 0  | 1  |
        // | 2  | 3  |
        // -----------
        // Y

        int64_t value = position.y() <= _halfY ? 0 : 2;
        value += position.x() <= _halfX ? 0 : 1;

        return value;
    }

    auto GameSpatialCell::GetSize() const -> int64_t
    {
        return std::accumulate(_entities.begin(), _entities.end(), int64_t{ 0 }, [](int64_t sum, const auto& container)
            {
                return sum + std::ssize(container);
            });
    }

    auto GameSpatialCell::GetSize(GameEntityType type) const -> int64_t
    {
        return std::ssize(GetContainer(type));
    }

    auto GameSpatialCell::GetEntities(GameEntityType type) -> std::span<game_entity_id_type>
    {
        return GetContainer(type);
    }

    auto GameSpatialCell::GetEntities(GameEntityType type) const -> std::span<const game_entity_id_type>
    {
        return GetContainer(type);
    }

    auto GameSpatialCell::GetEntities() -> std::ranges::join_view<std::ranges::ref_view<container_type>>
    {
        return _entities | std::views::join;
    }

    auto GameSpatialCell::GetEntities() const -> std::ranges::join_view<std::ranges::ref_view<const container_type>>
    {
        return _entities | std::views::join;
    }

    auto GameSpatialCell::GetContainer(GameEntityType type) -> std::vector<game_entity_id_type>&
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_entities));

        return _entities[index];
    }

    auto GameSpatialCell::GetContainer(GameEntityType type) const -> const std::vector<game_entity_id_type>&
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_entities));

        return _entities[index];
    }
}
