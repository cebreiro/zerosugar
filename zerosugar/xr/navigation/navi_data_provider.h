#pragma once
#include "zerosugar/xr/navigation/navi_data.h"

namespace zerosugar::xr
{
    class NavigationDataProvider final : public IService
    {
    public:
        void Initialize(ServiceLocator& serviceLocator, const std::filesystem::path& basePath);

        bool Contains(int32_t mapId) const;
        auto Create(int32_t mapId, int32_t maxSearchNode = 1024) const -> navi::Data;

        auto GetName() const -> std::string_view;

    private:
        static auto CreateData(const std::filesystem::path& filePath, int32_t maxSearchNode) -> navi::Data;

    private:
        std::filesystem::path _path;

        std::unordered_map<int32_t, std::filesystem::path> _naviFilePaths;
        std::unordered_map<int32_t, std::vector<navi::Data>> _naviData;
    };
}
