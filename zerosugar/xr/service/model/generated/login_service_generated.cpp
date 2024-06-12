#include "login_service_generated.h"

namespace zerosugar::xr::service
{
    auto GetEnumName(LoginServiceErrorCode e) -> std::string_view
    {
        switch (e)
        {
            case LoginServiceErrorCode::LoginErrorNone: return "LoginErrorNone";
            case LoginServiceErrorCode::LoginErrorFailInvalid: return "LoginErrorFailInvalid";
        assert(false);
        return "unk";
        }
    }
}
