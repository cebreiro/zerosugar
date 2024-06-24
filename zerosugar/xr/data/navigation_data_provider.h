#pragma once

class dtNavMesh;

namespace zerosugar::xr
{
    class NavigationDataProvider final : public IService
    {
    public:
        void Initialize(ServiceLocator& serviceLocator) override;
        void StartUp(const std::filesystem::path& directory);

        auto GetName() const -> std::string_view override;

    private:
        void AddNavMesh(const std::filesystem::path& filePath);

    private:
        ServiceLocatorT<ILogService> _serviceLocator;
        std::unordered_map<int32_t, SharedPtrNotNull<dtNavMesh>> _naviMeshes;
    };
}
