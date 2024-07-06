#pragma once

class dtNavMesh;
class dtNavMeshQuery;

namespace zerosugar::xr::navi
{
    class Data
    {
    public:
        Data() = default;
        Data(int32_t mapId, std::string objFilePath, SharedPtrNotNull<dtNavMesh> mesh, SharedPtrNotNull<dtNavMeshQuery> query);

        auto GetMapId() const -> int32_t;
        auto GetMesh() -> dtNavMesh&;
        auto GetMesh() const -> const dtNavMesh&;
        auto GetQuery() -> dtNavMeshQuery&;
        auto GetQuery() const -> const dtNavMeshQuery&;
        auto GetObjectFilePath() const -> const std::string&;

    private:
        int32_t _mapId = 0;
        SharedPtrNotNull<dtNavMesh> _mesh;
        SharedPtrNotNull<dtNavMeshQuery> _query;
        std::string _objectFilePath;
    };
}
