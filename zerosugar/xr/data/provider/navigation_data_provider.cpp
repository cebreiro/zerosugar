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
    static const int NAVMESHSET_VERSION = 1;
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
                data::Navigation naviData = CreateData(path, 1024);

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

    auto NavigationDataProvider::Create(int32_t mapId, int32_t maxSearchNode) -> data::Navigation
    {
        assert(Contains(mapId));

        return CreateData(_naviFilePaths[mapId], maxSearchNode);
    }

    auto NavigationDataProvider::CreateData(const std::filesystem::path& filePath, int32_t maxSearchNode) -> data::Navigation
    {
        std::string message;

        do
        {
            std::ifstream ifstream(filePath, std::ios::in | std::ios::binary);
            if (!ifstream.is_open())
            {
                message = "fail to open file";

                break;
            }

            NavMeshSetHeader header;
            ifstream.read(reinterpret_cast<char*>(&header), sizeof(header));

            if (header.magic != NAVMESHSET_MAGIC || header.version != NAVMESHSET_VERSION)
            {
                message = std::format("invalid file header");

                break;
            }

            std::shared_ptr<dtNavMesh> mesh(dtAllocNavMesh(), [](dtNavMesh* ptr)
                {
                    if (!ptr)
                    {
                        return;
                    }

                    dtFreeNavMesh(ptr);
                });

            const dtStatus status = mesh->init(&header.params);
            if (dtStatusFailed(status))
            {
                message = std::format("fail to init nav mesh");

                break;
            }

            for (int i = 0; i < header.numTiles; ++i)
            {
                NavMeshTileHeader tileHeader;
                ifstream.read(reinterpret_cast<char*>(&tileHeader), sizeof(tileHeader));

                if (!tileHeader.tileRef || !tileHeader.dataSize)
                {
                    break;
                }

                auto data = static_cast<char*>(dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM));
                if (!data)
                {
                    break;
                }

                boost::scope::scope_fail failGuard([data]()
                    {
                        dtFree(data);
                    });

                memset(data, 0, tileHeader.dataSize);
                ifstream.read(data, sizeof(tileHeader.dataSize));

                mesh->addTile(reinterpret_cast<uint8_t*>(data), tileHeader.dataSize,
                    DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
            }

            std::shared_ptr<dtNavMeshQuery> query(dtAllocNavMeshQuery(), [](dtNavMeshQuery* ptr)
                {
                    if (!ptr)
                    {
                        return;
                    }

                    dtFreeNavMeshQuery(ptr);
                });

            query->init(mesh.get(), maxSearchNode);
            (void)query;

            const int32_t id = boost::lexical_cast<int32_t>(filePath.stem().generic_string());

            return data::Navigation(id, std::move(mesh), std::move(query));

        } while (false);

        throw std::runtime_error(message);
    }

    auto NavigationDataProvider::GetName() const -> std::string_view
    {
        return "navigation_data_provider";
    }
}
