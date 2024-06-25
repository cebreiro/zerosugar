#include "game_spatial_container.h"

namespace zerosugar::xr
{
    GameSpatialContainer::GameSpatialContainer(int32_t width, int32_t height, int32_t lengthPerGrid)
        : _width(width)
        , _height(height)
        , _lengthPerGrid(lengthPerGrid)
    {
        assert(_lengthPerGrid > 0);

        Initialize();
    }

    GameSpatialContainer::~GameSpatialContainer()
    {
    }

    auto GameSpatialContainer::GetSector(int32_t x, int32_t y) -> GameSpatialSector&
    {
        const int64_t index = CalculateIndex(x, y);
        assert(index >= 0 && index < std::ssize(_sectors));

        return _sectors[index];
    }

    auto GameSpatialContainer::GetSector(int32_t x, int32_t y) const -> const GameSpatialSector&
    {
        const int64_t index = CalculateIndex(x, y);
        assert(index >= 0 && index < std::ssize(_sectors));

        return _sectors[index];
    }

    void GameSpatialContainer::Initialize()
    {
        const int32_t xSize = _width / _lengthPerGrid;
        const int32_t ySize = _height / _lengthPerGrid;

        const int32_t size = xSize * ySize;

        _cells.reserve(size);
        _sectors.reserve(size);

        [[maybe_unused]] const size_t cellsCapacity = _cells.capacity();
        [[maybe_unused]] const size_t sectorsCapacity = _sectors.capacity();

        for (int32_t y = 0; y < ySize; ++y)
        {
            for (int32_t x = 0; x < xSize; ++x)
            {
                _sectors.emplace_back(game_spatial_sector_id_type(x, y));
            }
        }

        auto getAdjacentSectors = [this, xSize, ySize](int32_t x, int32_t y) -> boost::container::static_vector<PtrNotNull<GameSpatialSector>, 9>
            {
                boost::container::static_vector<PtrNotNull<GameSpatialSector>, 9> result;

                for (int32_t j = y - 1; j <= y + 1; ++j)
                {
                    if (j < 0 || j >= ySize)
                    {
                        continue;
                    }

                    for (int32_t i = x - 1; i <= x + 1; ++i)
                    {
                        if (i < 0 || i >= xSize)
                        {
                            continue;
                        }

                        GameSpatialSector& sector = GetSector(i, j);

                        result.push_back(&sector);
                    }
                }

                return result;
            };

        for (int32_t y = 0; y < ySize; ++y)
        {
            for (int32_t x = 0; x < xSize; ++x)
            {
                GameSpatialCell& cell = _cells.emplace_back(game_spatial_cell_id_type(x, y));

                for (PtrNotNull<GameSpatialSector> sector : getAdjacentSectors(x, y))
                {
                    sector->AddCell(&cell);
                }
            }
        }

        assert(cellsCapacity == _cells.capacity());
        assert(cellsCapacity == _cells.size());
        assert(sectorsCapacity == _sectors.capacity());
        assert(sectorsCapacity == _sectors.size());
    }

    auto GameSpatialContainer::CalculateIndex(int32_t x, int32_t y) const -> int64_t
    {
        const int32_t clampedX = std::clamp(x, 0, _width);
        const int32_t clampedY = std::clamp(y, 0, _height);

        const int32_t ySize = _height / _lengthPerGrid;

        return clampedX + (clampedY * ySize);
    }
}
