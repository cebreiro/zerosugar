#include "login_service_message.h"

namespace zerosugar::xr::service
{
    auto GetEnumName(LoginServiceErrorCode e) -> std::string_view
    {
        switch (e)
        {
            case LoginServiceErrorCode::LoginErrorNone: return "LoginErrorNone";
            case LoginServiceErrorCode::LoginErrorFailInvalid: return "LoginErrorFailInvalid";
            case LoginServiceErrorCode::LoginErrorInternalError: return "LoginErrorInternalError";
            case LoginServiceErrorCode::AuthenticateErrorFail: return "AuthenticateErrorFail";
            case LoginServiceErrorCode::RemoveAuthErrorTokenNotFound: return "RemoveAuthErrorTokenNotFound";
        }
        assert(false);
        return "unk";
    }
}
