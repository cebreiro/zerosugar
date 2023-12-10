#pragma once
#include "zerosugar/shared/app/app_intance.h"
#include "zerosugar/sl/executable/monolithic_server/config/application_config.h"

namespace zerosugar::sl
{
    class IModule;

    class ServerApplication final : public AppInstance
    {
    public:
        ServerApplication();
        ~ServerApplication() override;

    private:
        void OnStartUp(std::span<char*> args) override;
        void OnShutdown() override;
        void OnExit(std::vector<boost::system::error_code>& errors) override;

    public:
        auto GetName() const -> std::string_view override;

    private:
        void InitializeConfig();
        void InitializeLogService();
        void InitializeModules();

    private:
        ServerConfig _config = {};
        std::vector<std::unique_ptr<IModule>> _modules;
    };
}
