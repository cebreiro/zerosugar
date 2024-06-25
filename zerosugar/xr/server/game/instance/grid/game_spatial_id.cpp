#include "game_spatial_id.h"

namespace zerosugar::xr::game::detail
{
    GameSpatialId::GameSpatialId(int32_t x, int32_t y)
        : _x(x)
        , _y(y)
    {
    }

    auto GameSpatialId::GetX() const -> int32_t
    {
        return _x;
    }

    auto GameSpatialId::GetY() const -> int32_t
    {
        return _y;
    }

    bool GameSpatialId::operator<(GameSpatialId other) const
    {
        if (_x == other._x)
        {
            return _y < other._y;
        }

        return _x < other._x;
    }

    bool GameSpatialId::operator==(GameSpatialId other) const
    {
        return _x == other._x && _y == other._y;
    }

    bool GameSpatialId::operator!=(GameSpatialId other) const
    {
        return _x != other._x || _y != other._y;
    }
}
