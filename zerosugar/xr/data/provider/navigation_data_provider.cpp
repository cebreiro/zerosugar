#include "navigation_data_provider.h"

#include <boost/lexical_cast.hpp>
#include <boost/scope/scope_fail.hpp>
#include "recastnavigation/DetourNavMesh.h"
#include "recastnavigation/DetourNavMeshQuery.h"

namespace
{
    struct NavMeshSetHeader
    {
        int magic;
        int version;
        int numTiles;
        dtNavMeshParams params;
    };

    struct NavMeshTileHeader
    {
        dtTileRef tileRef;
        int dataSize;
    };

    static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
    //static const int NAVMESHSET_VERSION = 1;
}

namespace zerosugar::xr
{
    void NavigationDataProvider::Initialize(ServiceLocator& serviceLocator, const std::filesystem::path& basePath)
    {
        const auto directory = basePath / "navigation";
        if (!exists(directory))
        {
            ZEROSUGAR_LOG_ERROR(serviceLocator,
                std::format("[{}] fail to find navigation directory. path: {}",
                    GetName(), directory.generic_string()));

            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(directory))
        {
            const auto& path = entry.path();

            if (!path.has_extension() || path.extension().string() != ".bin")
            {
                continue;
            }

            try
            {
                navi::Data naviData = CreateData(path, 1024);

                [[maybe_unused]]
                const bool inserted = _naviFilePaths.try_emplace(naviData.GetMapId(), path).second;
                assert(inserted);
            }
            catch (const std::exception& e)
            {
                ZEROSUGAR_LOG_ERROR(serviceLocator,
                    std::format("[{}] fail to load navigation data. exception: {}, path: {}",
                        GetName(), e.what(), path.generic_string()));
            }
        }
    }

    bool NavigationDataProvider::Contains(int32_t mapId) const
    {
        return _naviFilePaths.contains(mapId);
    }

    auto NavigationDataProvider::Create(int32_t mapId, int32_t maxSearchNode) const -> navi::Data
    {
        const auto iter = _naviFilePaths.find(mapId);
        assert(iter != _naviFilePaths.end());

        return CreateData(iter->second, maxSearchNode);
    }

    auto NavigationDataProvider::CreateData(const std::filesystem::path& filePath, int32_t maxSearchNode) -> navi::Data
    {
        std::string message;

        std::ifstream ifstream(filePath, std::ios::in | std::ios::binary);
        if (!ifstream.is_open())
        {
            throw std::runtime_error("fail to open file");
        }

        NavMeshSetHeader header;
        ifstream.read(reinterpret_cast<char*>(&header), sizeof(header));

        if (header.magic != DT_NAVMESH_MAGIC || header.version != DT_NAVMESH_VERSION)
        {
            throw std::runtime_error("invalid file header");
        }

        std::shared_ptr<dtNavMesh> mesh(dtAllocNavMesh(), [](dtNavMesh* ptr)
            {
                if (!ptr)
                {
                    return;
                }

                dtFreeNavMesh(ptr);
            });

        if (const dtStatus status = mesh->init(&header.params); dtStatusFailed(status))
        {
            throw std::runtime_error("fail to init nav mesh");
        }

        for (int32_t i = 0; i < header.numTiles; ++i)
        {
            NavMeshTileHeader tileHeader;
            ifstream.read(reinterpret_cast<char*>(&tileHeader), sizeof(tileHeader));
            assert(ifstream.gcount() == sizeof(tileHeader));

            if (!tileHeader.tileRef || !tileHeader.dataSize)
            {
                throw std::runtime_error("fail to init nav tile data");
            }

            auto data = static_cast<char*>(dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM));
            if (!data)
            {
                throw std::runtime_error("fail to init nav tile data");
            }

            boost::scope::scope_fail failGuard([data]()
                {
                    dtFree(data);
                });

            memset(data, 0, tileHeader.dataSize);

            ifstream.read(data, tileHeader.dataSize);
            assert(ifstream.gcount() == tileHeader.dataSize);

            if (const dtStatus status = mesh->addTile(reinterpret_cast<uint8_t*>(data), tileHeader.dataSize,
                DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
                dtStatusFailed(status))
            {
                throw std::runtime_error("fail to init nav tile data");
            }
        }

        std::shared_ptr<dtNavMeshQuery> query(dtAllocNavMeshQuery(), [](dtNavMeshQuery* ptr)
            {
                if (!ptr)
                {
                    return;
                }

                dtFreeNavMeshQuery(ptr);
            });

        if (const dtStatus status = query->init(mesh.get(), maxSearchNode); dtStatusFailed(status))
        {
            throw std::runtime_error("fail to init nav query");
        }

        const int32_t id = boost::lexical_cast<int32_t>(filePath.stem().generic_string());
        const std::filesystem::path objectFilePath = filePath.parent_path() / std::format("{}.obj", filePath.stem().generic_string());

        return navi::Data(id, objectFilePath.generic_string(), std::move(mesh), std::move(query));
    }

    auto NavigationDataProvider::GetName() const -> std::string_view
    {
        return "navigation_data_provider";
    }
}
