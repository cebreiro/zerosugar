#pragma once
#include "zerosugar/shared/app/app_intance.h"

namespace zerosugar::xr
{
    struct AllInOneServerConfig;

    class AllInOneServer final : public AppInstance
    {
    public:
        AllInOneServer() = delete;
        explicit AllInOneServer(const AllInOneServerConfig& config);

    private:
        void OnStartUp(std::span<char*> args) override;
        void OnShutdown() override;

    public:
        auto GetName() const -> std::string_view override;

    private:
    };
}
