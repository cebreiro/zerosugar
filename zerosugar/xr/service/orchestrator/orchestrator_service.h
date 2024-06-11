#pragma once
#include "zerosugar/xr/service/model/generated/orchestrator_service_generated_interface.h"

namespace zerosugar
{
    class Strand;
}

namespace zerosugar::xr
{
    class OrchestratorService final
        : public service::IOrchestratorService
        , public std::enable_shared_from_this<OrchestratorService>
    {
    public:
        explicit OrchestratorService(SharedPtrNotNull<Strand> strand);

        void Initialize(ServiceLocator& dependencyLocator) override;
        void Shutdown() override;
        void Join(std::vector<boost::system::error_code>& errors) override;

        auto RegisterAsync(service::RegisterParam param) -> Future<service::RegisterResult> override;
        auto PingAsync(service::PingParam param) -> Future<service::PingResult> override;

        auto CallRemoteProcedureAsync(service::CallRemoteProcedureParam param)
            -> Future<service::CallRemoteProcedureResult> override;

    private:
        SharedPtrNotNull<Strand> _strand;
    };
}
