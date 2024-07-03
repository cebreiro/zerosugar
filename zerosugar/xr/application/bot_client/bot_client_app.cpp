#include "bot_client_app.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger_builder.h"
#include "zerosugar/xr/application/bot_client/bot_client_app_config.h"
#include "zerosugar/xr/application/bot_client/controller/bot_control_service.h"
#include "zerosugar/xr/data/behavior_tree_xml_provider.h"
#include "zerosugar/xr/data/navigation_data_provider.h"
#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_interface.h"

namespace zerosugar::xr
{
    BotClientApp::BotClientApp()
        : _config(std::make_unique<BotClientAppConfig>())
        , _logService(std::make_shared<LogService>())
        , _behaviorTreeDataProvider(std::make_shared<BehaviorTreeXMLProvider>())
        , _navigationDataProvider(std::make_shared<NavigationDataProvider>())
    {
        ServiceLocator& serviceLocator = GetServiceLocator();

        serviceLocator.Add<ILogService>(_logService);
        serviceLocator.Add<BehaviorTreeXMLProvider>(_behaviorTreeDataProvider);
        serviceLocator.Add<NavigationDataProvider>(_navigationDataProvider);
    }

    BotClientApp::~BotClientApp()
    {
    }

    void BotClientApp::OnStartUp(std::span<char*> args)
    {
        (void)args;

        InitializeConfig();
        InitializeLogger();
        InitializeExecutor();

        ServiceLocator& serviceLocator = GetServiceLocator();

        InitializeGameData(serviceLocator);
        InitializeBotControlService();
    }

    void BotClientApp::OnShutdown()
    {
    }

    auto BotClientApp::GetName() const -> std::string_view
    {
        return "bot_client_application";
    }

    void BotClientApp::InitializeConfig()
    {
        auto path = std::filesystem::current_path();
        for (; exists(path) && path != path.parent_path(); path = path.parent_path())
        {
            std::filesystem::path configPath = path / "zerosugar/xr/application/bot_client/bot_client_app.config";
            if (!exists(configPath))
            {
                continue;
            }

            std::ifstream ifs(configPath);
            if (!ifs.is_open())
            {
                throw std::runtime_error("fail to open config file");
            }

            nlohmann::json json = nlohmann::json::parse(ifs);
            json.get_to<BotClientAppConfig>(*_config);

            return;
        }

        throw std::runtime_error("fail to initialize config");
    }

    void BotClientApp::InitializeLogger()
    {
        const std::filesystem::path& logFilePath = std::filesystem::current_path() / _config->logFilePath;

        SpdLogLoggerBuilder builder;
        builder.ConfigureConsole().SetLogLevel(LogLevel::Debug).SetAsync(false);
        builder.ConfigureDailyFile().SetLogLevel(LogLevel::Info).SetPath(logFilePath);

        _logService->Add(-1, builder.CreateLogger());

        GetServiceLocator().Add<ILogService>(_logService);

        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize logger --> Done. log_file_path: {}", GetName(), logFilePath.string()));
    }

    void BotClientApp::InitializeExecutor()
    {
        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize executor", GetName()));

        _executor = std::make_shared<execution::AsioExecutor>(_config->workerCount);

        ExecutionContext::PushExecutor(_executor.get());
        _executor->Run();

        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize executor --> Done", GetName()));
    }

    void BotClientApp::InitializeGameData(ServiceLocator& serviceLocator)
    {
        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize game data", GetName()));

        _behaviorTreeDataProvider->Initialize(serviceLocator);
        _behaviorTreeDataProvider->StartUp(_config->behaviorTreeDataDirectoryPath);

        _navigationDataProvider->Initialize(serviceLocator);
        _navigationDataProvider->StartUp(_config->navigationDataDirectoryPath);

        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize game data --> Done", GetName()));
    }

    void BotClientApp::InitializeBotControlService()
    {
        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize bot control service", GetName()));

        _botControlService = std::make_shared<BotControlService>(_executor, GetServiceLocator(),
            _config->workerCount, 1, "login");
        _botControlService->Start();

        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("[{}] initialize bot control service --> Done", GetName()));
    }
}
