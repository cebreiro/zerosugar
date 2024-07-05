#pragma once
#include "zerosugar/xr/data/provider/navi_data.h"

namespace zerosugar::xr
{
    class NavigationDataProvider final
    {
    public:
        void Initialize(ServiceLocator& serviceLocator, const std::filesystem::path& basePath);

        bool Contains(int32_t mapId) const;
        auto Create(int32_t mapId, int32_t maxSearchNode) -> data::Navigation;

        auto GetName() const->std::string_view;

    private:
        static auto CreateData(const std::filesystem::path& filePath, int32_t maxSearchNode) -> data::Navigation;

    private:
        std::filesystem::path _path;

        std::unordered_map<int32_t, std::filesystem::path> _naviFilePaths;
        std::unordered_map<int32_t, std::vector<data::Navigation>> _naviData;
    };
}
