#include "orchestrator_service.h"

#include "zerosugar/xr/network/rpc/rpc_client.h"
#include "zerosugar/xr/network/rpc/rpc_server.h"

namespace zerosugar::xr
{
    OrchestratorService::OrchestratorService(SharedPtrNotNull<execution::IExecutor> executor)
        : _executor(std::move(executor))
        , _strand(std::make_shared<Strand>(_executor))
    {
    }

    void OrchestratorService::Initialize(ServiceLocator& dependencyLocator)
    {
        IOrchestratorService::Initialize(dependencyLocator);

        ConfigureRemoteProcedureServer(dependencyLocator.Get<RPCServer>());
        ConfigureRemoteProcedureClient(dependencyLocator.Get<RPCClient>());
    }

    void OrchestratorService::StartUp()
    {
    }

    auto OrchestratorService::HandleCallRemoteProcedure(const network::RequestRemoteProcedureCall& request) -> Future<network::RemoteProcedureCallErrorCode>
    {
        (void)request;

        using enum network::RemoteProcedureCallErrorCode;

        // TODO: save request to redis if it is db rpc

        co_return RpcErrorNone;
    }

    auto OrchestratorService::HandleResultRemoteProcedure(const network::ResultRemoteProcedureCall& result) -> Future<void>
    {
        (void)result;

        // TODO remove request to redis if it is db rpc

        co_return;
    }

    void OrchestratorService::ConfigureRemoteProcedureServer(RPCServer& rpcServer)
    {
        _rpcServer = rpcServer.weak_from_this().lock();
    }

    void OrchestratorService::ConfigureRemoteProcedureClient(RPCClient& rpcClient)
    {
        (void)rpcClient;
    }
}
