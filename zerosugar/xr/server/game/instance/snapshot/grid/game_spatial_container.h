#pragma once
#include "zerosugar/xr/server/game/instance/snapshot/grid/game_spatial_cell.h"
#include "zerosugar/xr/server/game/instance/snapshot/grid/game_spatial_sector.h"

namespace zerosugar::xr
{
    class GameSpatialContainer
    {
    public:
        GameSpatialContainer(int32_t width, int32_t height, int32_t lengthPerGrid);
        ~GameSpatialContainer();

        auto GetSector(double x, double y) -> GameSpatialSector&;
        auto GetSector(double x, double y) const -> const GameSpatialSector&;

    private:
        void Initialize();

        auto GetSector(int32_t x, int32_t y) -> GameSpatialSector&;
        auto GetSector(int32_t x, int32_t y) const -> const GameSpatialSector&;

        auto CalculateXIndex(double x) const -> int32_t;
        auto CalculateYIndex(double y) const -> int32_t;
        auto CalculateIndex(int32_t x, int32_t y) const -> int64_t;

    private:
        int32_t _width = 0;
        int32_t _height = 0;
        int32_t _lengthPerGrid = 0;

        int32_t _xSize = 0;
        int32_t _ySize = 0;

        std::vector<GameSpatialCell> _cells;
        std::vector<GameSpatialSector> _sectors;
    };
}
