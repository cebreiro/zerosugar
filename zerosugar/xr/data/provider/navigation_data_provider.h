#pragma once

class dtNavMesh;

namespace zerosugar::xr
{
    class NavigationDataProvider final
    {
    public:
        void Initialize(ServiceLocator& serviceLocator, const std::filesystem::path& basePath);

        auto GetName() const -> std::string_view;

    private:
        void AddNavMesh(const std::filesystem::path& filePath);

    private:
        std::unordered_map<int32_t, SharedPtrNotNull<dtNavMesh>> _naviMeshes;
    };
}
