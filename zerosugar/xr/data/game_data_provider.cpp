#include "game_data_provider.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_interface.h"
#include "zerosugar/xr/data/provider/behavior_tree_xml_provider.h"
#include "zerosugar/xr/data/provider/map_data_provider.h"
#include "zerosugar/xr/data/provider/monster_data_provider.h"

namespace zerosugar::xr
{
    GameDataProvider::GameDataProvider()
        : _behaviorTreeXmlProvider(std::make_shared<BehaviorTreeXMLProvider>())
        , _mapDataProvider(std::make_shared<MapDataProvider>())
        , _monsterDataProvider(std::make_shared<MonsterDataProvider>())
    {
    }

    GameDataProvider::~GameDataProvider()
    {
    }

    void GameDataProvider::Initialize(ServiceLocator& serviceLocator)
    {
        const std::optional<std::filesystem::path> baseDirectory = FindGameDataBaseDirectory();
        if (!baseDirectory)
        {
            ZEROSUGAR_LOG_CRITICAL(serviceLocator,
                fmt::format("[{}] fail to find game data base directory.", GetName()));

            throw std::runtime_error("fail to initialize game data");
        }

        _baseDirectory = *baseDirectory;

        _behaviorTreeXmlProvider->Initialize(serviceLocator, _baseDirectory);
        _mapDataProvider->Initialize(serviceLocator, _baseDirectory);
        _monsterDataProvider->Initialize(serviceLocator, _baseDirectory);
    }

    auto GameDataProvider::Find(map_data_id_type mapId) const -> const MapData*
    {
        return _mapDataProvider->Find(mapId.Unwrap());
    }

    auto GameDataProvider::Find(monster_data_id_type monsterId) const -> const MonsterData*
    {
        return _monsterDataProvider->Find(monsterId.Unwrap());
    }

    auto GameDataProvider::FindBehaviorTree(const std::string& key) const -> const bt::INodeDataSet*
    {
        return _behaviorTreeXmlProvider->Find(key);
    }

    auto GameDataProvider::GetName() const -> std::string_view
    {
        return "game_data_provider";
    }

    auto GameDataProvider::GetBaseDirectory() const -> const std::filesystem::path&
    {
        return _baseDirectory;
    }

    auto GameDataProvider::FindGameDataBaseDirectory() -> std::optional<std::filesystem::path>
    {
        for (auto path = std::filesystem::current_path(); exists(path); path = path.parent_path())
        {
            const std::filesystem::path basePath = path / "zerosugar/xr/data/asset";
            if (exists(basePath))
            {
                return basePath;
            }
        }

        return std::nullopt;
    }
}
