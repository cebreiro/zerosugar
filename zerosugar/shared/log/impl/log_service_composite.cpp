#include "log_service_composite.h"

#include <cassert>
#include "zerosugar/shared/type/not_null_pointer.h"

namespace zerosugar
{
    void LogServiceComposite::Add(SharedPtrNotNull<LogService> logService)
    {
        _services.push_back(std::move(logService));
    }

    void LogServiceComposite::Log(LogLevel logLevel, const std::string& message, const std::source_location& location)
    {
        for (LogService& logService : _services | notnull::reference)
        {
            logService.Log(logLevel, message, location);
        }
    }

    void LogServiceComposite::Flush()
    {
        for (LogService& logService : _services | notnull::reference)
        {
            logService.Flush();
        }
    }
}
