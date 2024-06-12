#include "orchestrator_service_generated.h"

namespace zerosugar::xr::service
{
    auto GetEnumName(OrchestratorServiceErrorCode e) -> std::string_view
    {
        switch (e)
        {
            case OrchestratorServiceErrorCode::OrchestratorErrorNone: return "OrchestratorErrorNone";
            case OrchestratorServiceErrorCode::OrchestratorErrorFail: return "OrchestratorErrorFail";
            case OrchestratorServiceErrorCode::RegisterErrorDuplicatedServiceName: return "RegisterErrorDuplicatedServiceName";
            case OrchestratorServiceErrorCode::RegisterErrorDuplicatedServiceAddress: return "RegisterErrorDuplicatedServiceAddress";
            case OrchestratorServiceErrorCode::GetAddressErrorFailToFindService: return "GetAddressErrorFailToFindService";
        assert(false);
        return "unk";
        }
    }
}
