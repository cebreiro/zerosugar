#pragma once
#include "zerosugar/shared/app/app_intance.h"

namespace zerosugar::execution
{
    class AsioExecutor;
}

namespace zerosugar::xr
{
    struct BotClientAppConfig;

    class BehaviorTreeXMLProvider;
    class NavigationDataProvider;

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
        SharedPtrNotNull<execution::AsioExecutor> _executor;
        SharedPtrNotNull<LogService> _logService;

        SharedPtrNotNull<BehaviorTreeXMLProvider> _behaviorTreeDataProvider;
        SharedPtrNotNull<NavigationDataProvider> _navigationDataProvider;

        SharedPtrNotNull<BotControlService> _botControlService;
    };
}
