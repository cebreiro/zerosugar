#include "server_assembler.h"

#include "zerosugar/shared/app/app_intance.h"
#include "zerosugar/sl/executable/monolithic_server/config/application_config.h"
#include "zerosugar/sl/server/gateway/gateway_server.h"
#include "zerosugar/sl/server/login/login_server.h"
#include "zerosugar/sl/server/zone/zone_server.h"

namespace zerosugar::sl
{
    void ServerAssembler::Initialize(AppInstance& app, AppConfig& config)
    {
        const auto concurrency = static_cast<int64_t>(std::thread::hardware_concurrency());
        _executor = std::make_shared<execution::AsioExecutor>(concurrency);
        _executor->Run();

        InitializeServerExecutor();
        InitializeGatewayServers(app, config);
        InitializeZoneServers(app, config);
        InitializeLoginServers(app, config);
    }

    void ServerAssembler::Finalize() noexcept
    {
        for (const SharedPtrNotNull<Server>& server : _gatewayServers | std::views::values)
        {
            server->Shutdown();
        }

        for (const SharedPtrNotNull<Server>& server : _worldZoneServers | std::views::values | std::views::join)
        {
            server->Shutdown();
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

    void ServerAssembler::GetFinalizeError(std::vector<boost::system::error_code>& errors)
    {
        if (_executor)
        {
            _executor->Join(&errors);
        }
    }

    void ServerAssembler::InitializeServerExecutor()
    {
        const auto concurrency = static_cast<int64_t>(std::thread::hardware_concurrency());
        _executor = std::make_shared<execution::AsioExecutor>(concurrency);
        _executor->Run();
    }

    void ServerAssembler::InitializeGatewayServers(AppInstance& app, AppConfig& config)
    {
        auto gateways = [this, &config]() -> std::map<int8_t, SharedPtrNotNull<GatewayServer>>
            {
                std::map<int8_t, SharedPtrNotNull<GatewayServer>> result;

                for (const WorldConfig& worldConfig : config.GetServerConfig().GetWorldConfigs())
                {
                    const int8_t worldId = worldConfig.GetWorldId();

                    result[worldId] = std::make_shared<GatewayServer>(*_executor, worldId);
                }

                return result;
            }();

        for (auto& [worldId, gateway] : gateways)
        {
            gateway->Initialize(app.GetServiceLocator());
            gateway->StartUp();

            _gatewayServers[worldId] = std::move(gateway);
        }
    }

    void ServerAssembler::InitializeZoneServers(AppInstance& app, AppConfig& config)
    {
        auto worldZones = [&]() -> std::map<int8_t, std::vector<SharedPtrNotNull<ZoneServer>>>
            {
                std::map<int8_t, std::vector<SharedPtrNotNull<ZoneServer>>> result;

                for (const WorldConfig& worldConfig : config.GetServerConfig().GetWorldConfigs())
                {
                    const int8_t worldId = worldConfig.GetWorldId();
                    std::vector<SharedPtrNotNull<ZoneServer>>& servers = result[worldId];

                    for (const ZoneServerConfig& zoneConfig : worldConfig.GetZoneServerConfigs())
                    {
                        servers.push_back(std::make_shared<ZoneServer>(
                            *_executor, worldId, zoneConfig.GetZoneId(), zoneConfig.GetPort()));
                    }
                }

                return result;
            }();

        for (auto& [worldId, zones] : worldZones)
        {
            std::vector<SharedPtrNotNull<Server>>& zoneServers = _worldZoneServers[worldId];

            for (SharedPtrNotNull<ZoneServer>& zoneServer : zones)
            {
                zoneServer->Initialize(app.GetServiceLocator());
                zoneServer->StartUp();

                zoneServers.emplace_back(std::move(zoneServer));
            }
        }
    }

    void ServerAssembler::InitializeLoginServers(AppInstance& app, AppConfig& config)
    {
        (void)config;

        auto loginServer = std::make_shared<LoginServer>(*_executor);
        loginServer->Initialize(app.GetServiceLocator());
        loginServer->StartUp();

        _loginServer = std::move(loginServer);
    }
}
