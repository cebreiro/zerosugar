#pragma once
#include "zerosugar/xr/data/provider/map_data.h"

namespace zerosugar::xr
{
    class MapDataProvider final
    {
    public:
        void Initialize(ServiceLocator& serviceLocator, const std::filesystem::path& basePath);

        auto Find(int32_t mapId) const -> const data::Map*;

        auto GetName() const->std::string_view;

    private:
        std::unordered_map<int32_t, data::Map> _mapData;
    };
}
