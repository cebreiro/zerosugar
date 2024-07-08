#pragma once
#include "zerosugar/xr/data/table/map.h"

namespace zerosugar::xr
{
    class MapDataProvider final
    {
    public:
        void Initialize(ServiceLocator& serviceLocator, const std::filesystem::path& basePath);

        auto Find(int32_t mapId) const -> const MapData*;

        auto GetName() const -> std::string_view;

    private:
        std::unordered_map<int32_t, MapData> _mapData;
    };
}
