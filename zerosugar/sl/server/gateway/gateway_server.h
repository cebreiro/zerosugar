#pragma once
#include "zerosugar/shared/network/server/server.h"
#include "zerosugar/sl/service/generated/world_service_generated_interface.h"

namespace zerosugar::sl
{
    class ServerConfig;

    class GatewayServer final : public Server
    {
        using Server::StartUp;

    public:
        static constexpr uint16_t PORT = 2000;

        using locator_type = ServiceLocatorRef<ILogService, service::IWorldService>;

    public:
        GatewayServer(const GatewayServer& other) = delete;
        GatewayServer(GatewayServer&& other) noexcept = delete;
        GatewayServer& operator=(const GatewayServer& other) = delete;
        GatewayServer& operator=(GatewayServer&& other) noexcept = delete;

        GatewayServer(execution::AsioExecutor& executor, locator_type locator, const ServerConfig& config);
        ~GatewayServer();

        void StartUp();
        void Shutdown() override;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

    private:
        locator_type _locator;
        const ServerConfig& _config;
    };
}
