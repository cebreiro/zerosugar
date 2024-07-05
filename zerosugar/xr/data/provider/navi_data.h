#pragma once

class dtNavMesh;
class dtNavMeshQuery;

namespace zerosugar::xr::data
{
    class Navigation
    {
    public:
        Navigation() = default;
        Navigation(int32_t mapId, SharedPtrNotNull<dtNavMesh> mesh, SharedPtrNotNull<dtNavMeshQuery> query);

        auto GetMapId() const -> int32_t;
        auto GetMesh() const -> const dtNavMesh&;
        auto GetQuery() const -> const dtNavMeshQuery&;

    private:
        int32_t _mapId = 0;
        SharedPtrNotNull<dtNavMesh> _mesh;
        SharedPtrNotNull<dtNavMeshQuery> _query;
    };
}
