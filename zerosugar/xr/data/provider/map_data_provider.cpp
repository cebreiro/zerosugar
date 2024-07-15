#include "map_data_provider.h"

#include <boost/lexical_cast.hpp>

namespace zerosugar::xr
{
    void MapDataProvider::Initialize(ServiceLocator& serviceLocator, const std::filesystem::path& basePath)
    {
        const auto directory = basePath / "table" / "map";
        if (!exists(directory))
        {
            ZEROSUGAR_LOG_ERROR(serviceLocator,
                fmt::format("[{}] fail to find map directory. path: {}",
                    GetName(), directory.generic_string()));

            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(directory))
        {
            const auto& path = entry.path();

            if (!path.has_extension() || path.extension().string() != ".json")
            {
                continue;
            }

            try
            {
                std::ifstream ifstream(path, std::ios::in | std::ios::binary);
                if (!ifstream.is_open())
                {
                    throw std::runtime_error("fail to open");
                }

                nlohmann::json json;
                ifstream >> json;

                data::Map mapData;
                from_json(json, mapData);

                const int32_t id = boost::lexical_cast<int32_t>(path.stem().generic_string());

                [[maybe_unused]]
                const bool inserted = _mapData.try_emplace(id, std::move(mapData)).second;
                assert(inserted);
            }
            catch (const std::exception& e)
            {
                ZEROSUGAR_LOG_ERROR(serviceLocator,
                    fmt::format("[{}] fail to initialize map data. exception: {}, path: {}",
                        GetName(), e.what(), path.generic_string()));
            }
        }
    }

    auto MapDataProvider::Find(int32_t mapId) const -> const MapData*
    {
        const auto iter = _mapData.find(mapId);

        return iter != _mapData.end() ? &iter->second : nullptr;
    }

    auto MapDataProvider::GetName() const -> std::string_view
    {
        return "map_data_provider";
    }
}
