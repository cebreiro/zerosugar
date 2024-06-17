#include "coordination_service_message.h"

namespace zerosugar::xr::service
{
    auto GetEnumName(CoordinationServiceErrorCode e) -> std::string_view
    {
        switch (e)
        {
            case CoordinationServiceErrorCode::CoordinationErrorNone: return "CoordinationErrorNone";
            case CoordinationServiceErrorCode::CoordinationErrorInternalError: return "CoordinationErrorInternalError";
            case CoordinationServiceErrorCode::RequestSnowflakeKeyErrorOutOfPool: return "RequestSnowflakeKeyErrorOutOfPool";
            case CoordinationServiceErrorCode::ReturnSnowflakeKeyErrorInvalidKey: return "ReturnSnowflakeKeyErrorInvalidKey";
        }
        assert(false);
        return "unk";
    }
}
