#pragma once
#include "zerosugar/xr/data/game_data_id_type.h"

namespace zerosugar::bt
{
    class INodeDataSet;
}

namespace zerosugar::xr
{
    class BehaviorTreeXMLProvider;
    class MapDataProvider;
    class MonsterDataProvider;

    class MapData;
    class MonsterData;
}

namespace zerosugar::xr
{
    class GameDataProvider final
        : public IService
        , public std::enable_shared_from_this<GameDataProvider>
    {
    public:
        GameDataProvider();
        ~GameDataProvider();

        void Initialize(ServiceLocator& serviceLocator) override;

        auto Find(map_data_id_type mapId) const -> const MapData*;
        auto Find(monster_data_id_type monsterId) const -> const MonsterData*;

        auto FindBehaviorTree(const std::string& key) const -> const bt::INodeDataSet*;

        auto GetName() const -> std::string_view override;
        auto GetBaseDirectory() const -> const std::filesystem::path&;
        
    private:
        static auto FindGameDataBaseDirectory() -> std::optional<std::filesystem::path>;

    private:
        std::filesystem::path _baseDirectory;

        SharedPtrNotNull<BehaviorTreeXMLProvider> _behaviorTreeXmlProvider;
        SharedPtrNotNull<MapDataProvider> _mapDataProvider;
        SharedPtrNotNull<MonsterDataProvider> _monsterDataProvider;
    };
}
