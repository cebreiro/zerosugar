#pragma once
#include "zerosugar/xr/service/base/model/generated/orchestrator_service_generated_interface.h"

namespace zerosugar
{
    class Strand;
}

namespace zerosugar::xr
{
    class OrchestratorService
        : public service::IOrchestratorService
        , public std::enable_shared_from_this<OrchestratorService>
    {
    public:
        explicit OrchestratorService(SharedPtrNotNull<Strand> strand);

        void Initialize(ServiceLocator& dependencyLocator) override;
        void Shutdown() override;
        void Join(std::vector<boost::system::error_code>& errors) override;


    private:
        SharedPtrNotNull<Strand> _strand;
    };
}
