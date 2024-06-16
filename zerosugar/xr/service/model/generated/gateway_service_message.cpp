#include "gateway_service_message.h"

namespace zerosugar::xr::service
{
    auto GetEnumName(GatewayServiceErrorCode e) -> std::string_view
    {
        switch (e)
        {
            case GatewayServiceErrorCode::GatewayErrorNone: return "GatewayErrorNone";
            case GatewayServiceErrorCode::AddGameErrorNameDuplicated: return "AddGameErrorNameDuplicated";
        }
        assert(false);
        return "unk";
    }
}
