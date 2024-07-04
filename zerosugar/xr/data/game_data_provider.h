#pragma once

namespace zerosugar::xr
{
    class BehaviorTreeXMLProvider;
    class MapDataProvider;
    class NavigationDataProvider;
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

        auto GetName() const -> std::string_view override;

        auto GetBehaviorTreeXMLDataProvider() const -> const BehaviorTreeXMLProvider&;
        auto GetMapDataProvider() const -> const MapDataProvider&;
        auto GetNavigationDataProvider() const -> const NavigationDataProvider&;

    private:
        static auto FindGameDataBaseDirectory() -> std::optional<std::filesystem::path>;

    private:
        SharedPtrNotNull<BehaviorTreeXMLProvider> _behaviorTreeXmlProvider;
        SharedPtrNotNull<MapDataProvider> _mapDataProvider;
        SharedPtrNotNull<NavigationDataProvider> _navigationDataProvider;
    };
}
