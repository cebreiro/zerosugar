#include "coordination_service_message.h"

namespace zerosugar::xr::service
{
    auto GetEnumName(CoordinationServiceErrorCode e) -> std::string_view
    {
        switch (e)
        {
            case CoordinationServiceErrorCode::CoordinationErrorNone: return "CoordinationErrorNone";
            case CoordinationServiceErrorCode::CoordinationErrorInternalError: return "CoordinationErrorInternalError";
            case CoordinationServiceErrorCode::CoordinationErrorFailAuthentication: return "CoordinationErrorFailAuthentication";
            case CoordinationServiceErrorCode::RegisterErrorDuplicatedAddress: return "RegisterErrorDuplicatedAddress";
            case CoordinationServiceErrorCode::RequestSnowflakeKeyErrorOutOfPool: return "RequestSnowflakeKeyErrorOutOfPool";
            case CoordinationServiceErrorCode::ReturnSnowflakeKeyErrorInvalidKey: return "ReturnSnowflakeKeyErrorInvalidKey";
            case CoordinationServiceErrorCode::AuthenticatePlayerErrorUserNotFound: return "AuthenticatePlayerErrorUserNotFound";
            case CoordinationServiceErrorCode::AuthenticatePlayerErrorUserIsNotMigrating: return "AuthenticatePlayerErrorUserIsNotMigrating";
            case CoordinationServiceErrorCode::AuthenticatePlayerErrorRequestToInvalidServer: return "AuthenticatePlayerErrorRequestToInvalidServer";
        }
        assert(false);
        return "unk";
    }
}
