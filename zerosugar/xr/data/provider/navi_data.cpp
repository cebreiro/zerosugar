#include "navi_data.h"

namespace zerosugar::xr::data
{
    Navigation::Navigation(int32_t mapId, SharedPtrNotNull<dtNavMesh> mesh, SharedPtrNotNull<dtNavMeshQuery> query)
        : _mapId(mapId)
        , _mesh(std::move(mesh))
        , _query(std::move(query))
    {
    }

    auto Navigation::GetMapId() const -> int32_t
    {
        return _mapId;
    }

    auto Navigation::GetMesh() const -> const dtNavMesh&
    {
        return *_mesh;
    }

    auto Navigation::GetQuery() const -> const dtNavMeshQuery&
    {
        return *_query;
    }
}
