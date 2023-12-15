#include "server_module.h"

#include "zerosugar/shared/app/app_intance.h"
#include "zerosugar/sl/executable/monolithic_server/config/application_config.h"
#include "zerosugar/sl/server/gateway/gateway_server.h"
#include "zerosugar/sl/server/login/login_server.h"
#include "zerosugar/sl/server/zone/zone_server.h"

namespace zerosugar::sl
{
    void ServerModule::Initialize(AppInstance& app, AppConfig& config)
    {
        (void)config;

        const auto concurrency = static_cast<int64_t>(std::thread::hardware_concurrency());
        _executor = std::make_shared<execution::AsioExecutor>(concurrency);
        _executor->Run();

        const ServerConfig& serverConfig = config.GetServerConfig();

        auto loginServer = std::make_shared<LoginServer>(*_executor, app.GetServiceLocator(), serverConfig);
        auto gatewayServer = std::make_shared<GatewayServer>(*_executor, app.GetServiceLocator(), serverConfig);
        std::map<int8_t, std::vector<SharedPtrNotNull<ZoneServer>>> worldServers;

        for (const WorldConfig& worldConfig : config.GetServerConfig().GetWorldConfigs())
        {
            const int8_t worldId = worldConfig.GetWorldId();
            auto& zoneServers = worldServers[worldId];

            for (const ZoneServerConfig& zoneConfig : worldConfig.GetZoneServerConfigs())
            {
                zoneServers.push_back(std::make_shared<ZoneServer>(*_executor, app.GetServiceLocator(),
                    worldId, zoneConfig.GetZoneId(), zoneConfig.GetPort()));
            }
        }

        for (auto& [worldId, servers] : worldServers)
        {
            auto& dest = _worldServers[worldId];

            for (SharedPtrNotNull<ZoneServer>& zoneServer : servers)
            {
                zoneServer->StartUp();
                dest.emplace_back(std::move(zoneServer));
            }
        }

        gatewayServer->StartUp();
        _gatewayServer = std::move(gatewayServer);

        loginServer->StartUp();
        _loginServer = std::move(loginServer);
    }

    void ServerModule::Finalize() noexcept
    {
        for (const SharedPtrNotNull<Server>& server : _worldServers | std::views::values | std::views::join)
        {
            server->Shutdown();
        }

        if (_gatewayServer)
        {
            _gatewayServer->Shutdown();
        }

        if (_loginServer)
        {
            _loginServer->Shutdown();
        }

        if (_executor)
        {
            _executor->Stop();
        }
    }

    void ServerModule::GetFinalizeError(std::vector<boost::system::error_code>& errors)
    {
        if (_executor)
        {
            _executor->Join(&errors);
        }
    }
}
