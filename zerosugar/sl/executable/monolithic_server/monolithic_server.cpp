#include "monolithic_server.h"

#include <nlohmann/json.hpp>
#include "zerosugar/shared/service/service_locator_log.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger_builder.h"
#include "zerosugar/sl/executable/monolithic_server/assembler/server_assembler.h"
#include "zerosugar/sl/executable/monolithic_server/assembler/service_assembler.h"

namespace zerosugar::sl
{
    MonolithicServerApplication::MonolithicServerApplication()
    {
        _assemblers.emplace_back(std::make_unique<ServiceAssembler>());
        _assemblers.emplace_back(std::make_unique<ServerAssembler>());
    }

    MonolithicServerApplication::~MonolithicServerApplication()
    {
    }

    void MonolithicServerApplication::OnStartUp(std::span<char*> args)
    {
        (void)args;


        InitializeConfig();
        InitializeLogService();
        InitializeService();
    }

    void MonolithicServerApplication::OnShutdown()
    {
        for (IAssembler& assembler : _assemblers | notnull::reference)
        {
            assembler.Finalize();
        }
    }

    void MonolithicServerApplication::OnExit(std::vector<boost::system::error_code>& errors)
    {
        for (IAssembler& assembler : _assemblers | notnull::reference)
        {
            assembler.GetFinalizeError(errors);
        }

        for (const boost::system::error_code& error : errors)
        {
            ZEROSUGAR_LOG_CRITICAL(GetServiceLocator(),
                std::format("[{}] exit with error. error: [{}, {}]",
                    GetName(), error.value(), error.message()));
        }
    }

    auto MonolithicServerApplication::GetName() const -> std::string_view
    {
        return "sl_monolithic_server";
    }

    void MonolithicServerApplication::InitializeConfig()
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

    void MonolithicServerApplication::InitializeLogService()
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

    void MonolithicServerApplication::InitializeService()
    {
        for (IAssembler& assembler : _assemblers | notnull::reference)
        {
            assembler.Initialize(*this, _config);
        }
    }
}
