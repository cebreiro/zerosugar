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

    auto GameSpatialContainer::GetSector(double x, double y) -> GameSpatialSector&
    {
        return GetSector(CalculateXIndex(x), CalculateYIndex(y));
    }

    auto GameSpatialContainer::GetSector(double x, double y) const -> const GameSpatialSector&
    {
        return GetSector(CalculateXIndex(x), CalculateYIndex(y));
    }

    void GameSpatialContainer::Initialize()
    {
        _xSize = _width / _lengthPerGrid;
        _ySize = _height / _lengthPerGrid;

        const int32_t size = _xSize * _ySize;

        _cells.reserve(size);
        _sectors.reserve(size);

        [[maybe_unused]] const size_t cellsCapacity = _cells.capacity();
        [[maybe_unused]] const size_t sectorsCapacity = _sectors.capacity();

        for (int32_t y = 0; y < _ySize; ++y)
        {
            for (int32_t x = 0; x < _xSize; ++x)
            {
                _sectors.emplace_back(game_spatial_sector_id_type(x, y));
            }
        }

        auto getAdjacentSectors = [this](int32_t x, int32_t y) -> boost::container::static_vector<PtrNotNull<GameSpatialSector>, 9>
            {
                boost::container::static_vector<PtrNotNull<GameSpatialSector>, 9> result;

                for (int32_t j = y - 1; j <= y + 1; ++j)
                {
                    if (j < 0 || j >= _ySize)
                    {
                        continue;
                    }

                    for (int32_t i = x - 1; i <= x + 1; ++i)
                    {
                        if (i < 0 || i >= _xSize)
                        {
                            continue;
                        }

                        GameSpatialSector& sector = GetSector(i, j);

                        result.push_back(&sector);
                    }
                }

                return result;
            };

        for (int32_t y = 0; y < _ySize; ++y)
        {
            for (int32_t x = 0; x < _xSize; ++x)
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

    auto GameSpatialContainer::CalculateXIndex(double x) const -> int32_t
    {
        const int32_t intValue = static_cast<int32_t>(x);
        const int32_t clamped = std::clamp(intValue, 0, _width);

        return clamped / _lengthPerGrid;
    }

    auto GameSpatialContainer::CalculateYIndex(double y) const -> int32_t
    {
        const int32_t intValue = static_cast<int32_t>(y);
        const int32_t clamped = std::clamp(intValue, 0, _height);

        return clamped / _lengthPerGrid;
    }

    auto GameSpatialContainer::CalculateIndex(int32_t x, int32_t y) const -> int64_t
    {
        assert(x >= 0 && x < _xSize);
        assert(y >= 0 && y < _ySize);

        return x + (y * _ySize);
    }
}
