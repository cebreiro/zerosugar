#include "database_service_message.h"

namespace zerosugar::xr::service
{
    auto GetEnumName(DatabaseServiceErrorCode e) -> std::string_view
    {
        switch (e)
        {
            case DatabaseServiceErrorCode::DatabaseErrorNone: return "DatabaseErrorNone";
            case DatabaseServiceErrorCode::DatabaseErrorInternalError: return "DatabaseErrorInternalError";
            case DatabaseServiceErrorCode::AddAccountErrorDuplicated: return "AddAccountErrorDuplicated";
            case DatabaseServiceErrorCode::GetAccountErrorNotFound: return "GetAccountErrorNotFound";
        }
        assert(false);
        return "unk";
    }
}
