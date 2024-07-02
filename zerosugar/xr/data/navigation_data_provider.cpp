#include "navigation_data_provider.h"

#include <boost/lexical_cast.hpp>
#include <boost/scope/scope_fail.hpp>
#include "recastnavigation/DetourNavMesh.h"

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
    void NavigationDataProvider::Initialize(ServiceLocator& serviceLocator)
    {
        _serviceLocator = serviceLocator;
    }

    void NavigationDataProvider::StartUp(const std::filesystem::path& directory)
    {
        for (const auto& entry : std::filesystem::directory_iterator(directory))
        {
            const auto& path = entry.path();

            if (!path.has_extension() || path.extension().string() != ".bin")
            {
                continue;
            }

            try
            {
                AddNavMesh(path);
            }
            catch (const std::exception& e)
            {
                ZEROSUGAR_LOG_ERROR(_serviceLocator,
                    std::format("[{}] fail to load navigation data. exception: {}, path: {}",
                        GetName(), e.what(), path.generic_string()));
            }
        }
    }

    void NavigationDataProvider::AddNavMesh(const std::filesystem::path& filePath)
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


            const int32_t id = boost::lexical_cast<int32_t>(filePath.stem().generic_string());

            [[maybe_unused]]
            const bool inserted = _naviMeshes.try_emplace(id, std::move(mesh)).second;
            assert(inserted);

            return;

        } while (false);

        throw std::runtime_error(message);
    }

    auto NavigationDataProvider::GetName() const -> std::string_view
    {
        return "navigation_data_provider";
    }
}
