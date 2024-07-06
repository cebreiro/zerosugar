#include "navi_query.h"

#include <random>
#include <recastnavigation/DetourNavMeshQuery.h>
#include "zerosugar/xr/navigation/navi_vector.h"

namespace zerosugar::xr::navi
{
    auto FindNearestPoly(const dtNavMeshQuery& query, const Vector& center, const Vector& extents, const dtQueryFilter& filter)
        -> std::pair<dtPolyRef, Vector>
    {
        std::pair<dtPolyRef, Vector> result;

        query.findNearestPoly(center.GetData(), extents.GetData(), &filter, &result.first, result.second.GetData());

        return result;
    }

    auto GetRandomPointAroundCircle(const dtNavMeshQuery& query, const Vector& center, const Vector& extent, float radius, const dtQueryFilter& filter)
        -> std::pair<dtPolyRef, Vector>
    {
        std::pair<dtPolyRef, Vector> result;

        [[maybe_unused]]
        const auto [startPoly, nearestPoint] = FindNearestPoly(query, center, extent, filter);
        if (!startPoly)
        {
            return result;
        }

        query.findRandomPointAroundCircle(startPoly, center.GetData(), radius, &filter, []() -> float
            {
                thread_local std::mt19937 mt(std::random_device{}());
                thread_local std::uniform_real_distribution<float> dist(0.f, 1.f);

                return dist(mt);

            }, &result.first, result.second.GetData());

        return result;
    }

    auto FindStraightPath(const dtNavMeshQuery& query, const Vector& start, const Vector& end, const Vector& extents, const dtQueryFilter& filter)
        -> boost::container::static_vector<Vector, constant::max_straight_path_count>
    {
        const auto [startPoly, startNearestPoint] = FindNearestPoly(query, start, extents, filter);
        if (!startPoly)
        {
            return {};
        }

        const auto [endPoly, endNearestPoint] = FindNearestPoly(query, end, extents, filter);
        if (!endPoly)
        {
            return {};
        }

        std::array<dtPolyRef, constant::max_find_path_max_polygon> paths = {};
        int32_t pathCount = 0;

        if (const dtStatus status = query.findPath(startPoly, endPoly, startNearestPoint.GetData(),
            endNearestPoint.GetData(), &filter, paths.data(), &pathCount, constant::max_find_path_max_polygon);
            !dtStatusSucceed(status))
        {
            return {};
        }

        using points_type = std::array<float, 3>;
        static_assert(sizeof(points_type) == sizeof(float) * 3);

        std::array<points_type, constant::max_straight_path_count> straightPaths = {};
        int32_t straightPathCount = 0;

        constexpr unsigned char* straightPathFlags = nullptr;
        constexpr dtPolyRef* straightPathRefs = nullptr;

        if (const dtStatus status = query.findStraightPath(
            startNearestPoint.GetData(), endNearestPoint.GetData(),
            paths.data(), pathCount,
            straightPaths[0].data(), straightPathFlags, straightPathRefs,
            &straightPathCount, constant::max_straight_path_count);
            !dtStatusSucceed(status))
        {
            return {};
        }

        boost::container::static_vector<Vector, constant::max_straight_path_count> result;
        for (int32_t i = 0; i < straightPathCount; ++i)
        {
            result.emplace_back(straightPaths[i]);
        }

        return result;
    }
}
