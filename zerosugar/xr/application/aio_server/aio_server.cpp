#include "aio_server.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/xr/application/aio_server/aio_server_config.h"
#include "zerosugar/xr/network/rpc_client.h"
#include "zerosugar/xr/network/rpc_server.h"
#include "zerosugar/xr/service/orchestrator/orchestrator_service.h"
#include "zerosugar/xr/service/login/login_service.h"
#include "zerosugar/xr/service/login/login_service_proxy.h"

namespace zerosugar::xr
{
    AIOServer::AIOServer(const AIOServerConfig& config)
        : _config(std::make_unique<AIOServerConfig>(config))
        , _executor(std::make_shared<execution::AsioExecutor>(_config->workerCount))
        , _rpcServer(std::make_shared<RPCServer>(_executor))
        , _rpcClient(std::make_shared<RPCClient>(_executor))
        , _orchestratorService(std::make_shared<OrchestratorService>(_executor))
        , _loginService(std::make_shared<LoginService>(_executor))
        , _loginServiceProxy(std::make_shared<LoginServiceProxy>(_rpcClient))
    {
        ServiceLocator& serviceLocator = GetServiceLocator();

        serviceLocator.Add<RPCServer>(_rpcServer);
        serviceLocator.Add<RPCClient>(_rpcClient);

        serviceLocator.Add<service::IOrchestratorService>(_orchestratorService);
        serviceLocator.Add<service::ILoginService>(_loginServiceProxy);
    }

    void AIOServer::OnStartUp(std::span<char*> args)
    {
        (void)args;

        ServiceLocator& serviceLocator = GetServiceLocator();

        _rpcServer->Initialize(serviceLocator);
        _rpcClient->Initialize(serviceLocator);

        _orchestratorService->Initialize(serviceLocator);
        _loginService->Initialize(serviceLocator);

        _rpcServer->StartUp(_config->rpcServerPort);
        Future<void> future = _rpcClient->ConnectAsync(_config->rpcServerIP, _config->rpcServerPort);

        while (future.IsPending())
        {
            std::this_thread::sleep_for(std::chrono::seconds(3));

            // TODO: LOG
        }

        future.Get();

        //_orchestratorService->StartUp();
    }

    void AIOServer::OnShutdown()
    {
    }

    auto AIOServer::GetName() const -> std::string_view
    {
        return "AllInOneServer";
    }
}
