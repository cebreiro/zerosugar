#include "game_data_provider.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_interface.h"
#include "zerosugar/xr/data/provider/behavior_tree_xml_provider.h"
#include "zerosugar/xr/data/provider/map_data_provider.h"
#include "zerosugar/xr/data/provider/monster_data_provider.h"
#include "zerosugar/xr/data/provider/navigation_data_provider.h"

namespace zerosugar::xr
{
    GameDataProvider::GameDataProvider()
        : _behaviorTreeXmlProvider(std::make_shared<BehaviorTreeXMLProvider>())
        , _mapDataProvider(std::make_shared<MapDataProvider>())
        , _navigationDataProvider(std::make_shared<NavigationDataProvider>())
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
                std::format("[{}] fail to find game data base directory.", GetName()));

            throw std::runtime_error("fail to initialize game data");
        }

        _behaviorTreeXmlProvider->Initialize(serviceLocator, *baseDirectory);
        _mapDataProvider->Initialize(serviceLocator, *baseDirectory);
        _navigationDataProvider->Initialize(serviceLocator, *baseDirectory);
        _monsterDataProvider->Initialize(serviceLocator, *baseDirectory);
    }

    auto GameDataProvider::GetName() const -> std::string_view
    {
        return "game_data_provider";
    }

    auto GameDataProvider::GetBehaviorTreeXMLDataProvider() const -> const BehaviorTreeXMLProvider&
    {
        return *_behaviorTreeXmlProvider;
    }

    auto GameDataProvider::GetMapDataProvider() const -> const MapDataProvider&
    {
        return *_mapDataProvider;
    }

    auto GameDataProvider::GetNavigationDataProvider() const -> const NavigationDataProvider&
    {
        return *_navigationDataProvider;
    }

    auto GameDataProvider::GetMonsterDataProvider() const -> const MonsterDataProvider&
    {
        return *_monsterDataProvider;
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
