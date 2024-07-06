#include "navi_data.h"

namespace zerosugar::xr::navi
{
    Data::Data(int32_t mapId, std::string objFilePath, SharedPtrNotNull<dtNavMesh> mesh, SharedPtrNotNull<dtNavMeshQuery> query)
        : _mapId(mapId)
        , _mesh(std::move(mesh))
        , _query(std::move(query))
        , _objectFilePath(std::move(objFilePath))
    {
    }

    auto Data::GetMapId() const -> int32_t
    {
        return _mapId;
    }

    auto Data::GetMesh() -> dtNavMesh&
    {
        return *_mesh;
    }

    auto Data::GetMesh() const -> const dtNavMesh&
    {
        return *_mesh;
    }

    auto Data::GetQuery() -> dtNavMeshQuery&
    {
        return *_query;
    }

    auto Data::GetQuery() const -> const dtNavMeshQuery&
    {
        return *_query;
    }

    auto Data::GetObjectFilePath() const -> const std::string&
    {
        return _objectFilePath;
    }
}
