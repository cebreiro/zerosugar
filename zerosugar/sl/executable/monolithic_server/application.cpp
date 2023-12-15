#include "application.h"

#include <nlohmann/json.hpp>
#include "zerosugar/shared/service/service_locator_log.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger_builder.h"
#include "zerosugar/sl/executable/monolithic_server/module/server_module.h"
#include "zerosugar/sl/executable/monolithic_server/module/service_module.h"

namespace zerosugar::sl
{
    ServerApplication::ServerApplication()
    {
        _modules.emplace_back(std::make_unique<ServiceModule>());
        _modules.emplace_back(std::make_unique<ServerModule>());
    }

    ServerApplication::~ServerApplication()
    {
    }

    void ServerApplication::OnStartUp(std::span<char*> args)
    {
        (void)args;


        InitializeConfig();
        InitializeLogService();
        InitializeModules();
    }

    void ServerApplication::OnShutdown()
    {
        for (IModule& module : _modules | notnull::reference)
        {
            module.Finalize();
        }
    }

    void ServerApplication::OnExit(std::vector<boost::system::error_code>& errors)
    {
        for (IModule& module : _modules | notnull::reference)
        {
            module.GetFinalizeError(errors);
        }

        for (const boost::system::error_code& error : errors)
        {
            ZEROSUGAR_LOG_CRITICAL(GetServiceLocator(),
                std::format("[{}] exit with error. error: [{}, {}]",
                    GetName(), error.value(), error.message()));
        }
    }

    auto ServerApplication::GetName() const -> std::string_view
    {
        return "sl_monolithic_server";
    }

    void ServerApplication::InitializeConfig()
    {
        for (auto path = std::filesystem::current_path();
            exists(path) && path != path.parent_path();
            path = path.parent_path())
        {
            std::filesystem::path configPath = path / "zerosugar/sl/executable/monolithic_server/config/application.config";
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
            json.get_to<AppConfig>(_config);

            return;
        }

        throw std::runtime_error("fail to initialize config");
    }

    void ServerApplication::InitializeLogService()
    {
        const LogConfig& config = _config.GetLogConfig();
        const LogConfig::Console& console = config.GetConsole();
        const LogConfig::DailyFile& dailyFile = config.GetDailyFile();

        SpdLogLoggerBuilder builder;
        builder.ConfigureConsole()
            .SetAsync(console.ShouldAsync())
            .SetLogLevel(ToLogLevel(console.GetLevel()));

        const auto& dailyFileConfig = builder.ConfigureDailyFile()
            .SetPath(std::filesystem::current_path() / dailyFile.GetFilePath())
            .SetLogLevel(LogLevel::Debug);

        auto service = std::make_shared<LogService>();
        service->Add(0, builder.CreateLogger());

        GetServiceLocator().Add<ILogService>(std::move(service));

        ZEROSUGAR_LOG_INFO(GetServiceLocator(), std::format("file log path: {}", dailyFileConfig.GetPath().string()));
    }

    void ServerApplication::InitializeModules()
    {
        for (IModule& module : _modules | notnull::reference)
        {
            module.Initialize(*this, _config);
        }
    }
}
