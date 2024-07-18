#pragma once
#include <utility>
#include <boost/container/static_vector.hpp>
#include <recastnavigation/DetourNavMesh.h>

class dtNavMeshQuery;
class dtQueryFilter;

namespace zerosugar::xr::navi
{
    class Vector;
    class FVector;
    class Extents;
    class Scalar;
}

namespace zerosugar::xr::navi
{
    namespace constant
    {
        constexpr int32_t max_find_path_max_polygon = 256;
        constexpr int32_t max_straight_path_count = 8;
    }

    auto FindNearestPoly(const dtNavMeshQuery& query, const Vector& center, const Extents& extents, const dtQueryFilter& filter)
        -> std::pair<dtPolyRef, Vector>;

    auto GetRandomPoint(const dtNavMeshQuery& query, const dtQueryFilter& filter) -> std::pair<dtPolyRef, Vector>;

    auto GetRandomPointAroundCircle(const dtNavMeshQuery& query, const Vector& center, const Extents& extents, const Scalar& radius, const dtQueryFilter& filter)
        -> std::pair<dtPolyRef, Vector>;

    auto FindStraightPath(const dtNavMeshQuery& query, const Vector& start, const Vector& end, const Extents& extent, const dtQueryFilter& filter)
        -> boost::container::static_vector<Vector, constant::max_straight_path_count>;
}
