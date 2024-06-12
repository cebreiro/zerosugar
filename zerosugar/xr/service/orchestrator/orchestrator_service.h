#pragma once
#include "zerosugar/xr/service/model/generated/orchestrator_service_generated_interface.h"

namespace zerosugar::xr
{
    class RPCServer;
}

namespace zerosugar::xr::network
{
    struct RequestRemoteProcedureCall;
    struct ResultRemoteProcedureCall;

    enum class RemoteProcedureCallErrorCode;
}

namespace zerosugar::xr
{
    class RPCClient;

    class OrchestratorService final
        : public service::IOrchestratorService
        , public std::enable_shared_from_this<OrchestratorService>
    {
    public:
        explicit OrchestratorService(SharedPtrNotNull<execution::IExecutor> executor);

        void Initialize(ServiceLocator& dependencyLocator) override;
        void Shutdown() override;
        void Join(std::vector<boost::system::error_code>& errors) override;

        void StartUp();

        auto HandleCallRemoteProcedure(const network::RequestRemoteProcedureCall& request) -> Future<network::RemoteProcedureCallErrorCode>;
        auto HandleResultRemoteProcedure(const network::ResultRemoteProcedureCall& result) -> Future<void>;

    private:
        void ConfigureRemoteProcedureServer(RPCServer& server);
        void ConfigureRemoteProcedureClient(RPCClient& rpcClient);

    private:
        SharedPtrNotNull<execution::IExecutor> _executor;
        SharedPtrNotNull<Strand> _strand;

        WeakPtrNotNull<RPCServer> _rpcServer;
    };
}
