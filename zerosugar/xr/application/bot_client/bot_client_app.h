#pragma once
#include "zerosugar/shared/app/app_intance.h"

namespace zerosugar::execution
{
    class AsioExecutor;
    class GameExecutor;
}

namespace zerosugar::xr
{
    class GameDataProvider;
    class NavigationDataProvider;

    struct BotClientAppConfig;
    class BotControlService;
}

namespace zerosugar::xr
{
    class BotClientApp : public AppInstance
    {
    public:
        BotClientApp();
        ~BotClientApp();

        void OnStartUp(std::span<char*> args) override;
        void OnShutdown() override;

        auto GetName() const -> std::string_view override;

    private:
        void InitializeConfig();
        void InitializeLogger();
        void InitializeExecutor();
        void InitializeGameData(ServiceLocator& serviceLocator);
        void InitializeBotControlService();

    private:
        UniquePtrNotNull<BotClientAppConfig> _config;
        SharedPtrNotNull<execution::AsioExecutor> _ioExecutor;
        SharedPtrNotNull<execution::GameExecutor> _gameExecutor;
        SharedPtrNotNull<LogService> _logService;

        SharedPtrNotNull<GameDataProvider> _gameDataProvider;
        SharedPtrNotNull<NavigationDataProvider> _navigationDataProvider;

        SharedPtrNotNull<BotControlService> _botControlService;
    };
}
