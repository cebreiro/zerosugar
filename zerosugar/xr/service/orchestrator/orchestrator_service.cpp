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

    void OrchestratorService::Initialize(ServiceLocator& serviceLocator)
    {
        IOrchestratorService::Initialize(serviceLocator);

        ConfigureRemoteProcedureServer(serviceLocator.Get<RPCServer>());
        ConfigureRemoteProcedureClient(serviceLocator.Get<RPCClient>());
    }

    void OrchestratorService::Shutdown()
    {
        IOrchestratorService::Shutdown();
    }

    void OrchestratorService::Join(std::vector<boost::system::error_code>& errors)
    {
        IOrchestratorService::Join(errors);
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
        rpcServer.SetRequestHandler([self = shared_from_this()](const network::RequestRemoteProcedureCall& request)
            -> Future<network::RemoteProcedureCallErrorCode>
            {
                return self->HandleCallRemoteProcedure(request);
            });

        rpcServer.SetResultHandler([self = shared_from_this()](const network::ResultRemoteProcedureCall& result)
            -> Future<void>
            {
                return self->HandleResultRemoteProcedure(result);
            });

        _rpcServer = rpcServer.weak_from_this().lock();
    }

    void OrchestratorService::ConfigureRemoteProcedureClient(RPCClient& rpcClient)
    {
        (void)rpcClient;
    }
}
