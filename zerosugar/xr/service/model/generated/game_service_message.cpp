#include "game_service_message.h"

namespace zerosugar::xr::service
{
    auto GetEnumName(GameServiceErrorCode e) -> std::string_view
    {
        switch (e)
        {
            case GameServiceErrorCode::GameErrorNone: return "GameErrorNone";
            case GameServiceErrorCode::GameErrorInternalError: return "GameErrorInternalError";
            case GameServiceErrorCode::RequestSnowflakeKeyErrorOutOfPool: return "RequestSnowflakeKeyErrorOutOfPool";
            case GameServiceErrorCode::ReturnSnowflakeKeyErrorInvalidKey: return "ReturnSnowflakeKeyErrorInvalidKey";
        }
        assert(false);
        return "unk";
    }
}
