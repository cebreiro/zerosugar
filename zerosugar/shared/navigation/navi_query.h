#pragma once
#include <utility>
#include <vector>
#include <boost/container/static_vector.hpp>
#include <recastnavigation/DetourNavMesh.h>
#include "zerosugar/shared/navigation/navi_vector.h"

class dtNavMeshQuery;
class dtQueryFilter;

namespace zerosugar::navi
{
    class Vector;
}

namespace zerosugar::navi
{
    namespace constant
    {
        constexpr int32_t max_find_path_max_polygon = 256;
        constexpr int32_t max_straight_path_count = 8;
    }

    auto FindNearestPoly(const dtNavMeshQuery& query, const Vector& center, const Vector& extent, const dtQueryFilter& filter)
        -> std::pair<dtPolyRef, Vector>;

    auto GetRandomPointAroundCircle(const dtNavMeshQuery& query, const Vector& center, const Vector& extent, float radius, const dtQueryFilter& filter)
        -> std::pair<dtPolyRef, Vector>;

    auto FindStraightPath(const dtNavMeshQuery& query, const Vector& start, const Vector& end, const Vector& extent, const dtQueryFilter& filter)
        -> boost::container::static_vector<Vector, constant::max_straight_path_count>;
}
