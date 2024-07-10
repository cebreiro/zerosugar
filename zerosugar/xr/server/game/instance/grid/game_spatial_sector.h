#pragma once
#include "zerosugar/xr/server/game/instance/grid/game_spatial_set.h"

namespace zerosugar::xr
{
    class GameSpatialSector final : public GameSpatialSet
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
