#pragma once
#include <cstdint>
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/log/logger_interface.h"
#include "zerosugar/shared/type/not_null_pointer.h"

namespace zerosugar
{
    class ILogService : public ILogger, public IService
    {
    public:
        virtual ~ILogService() = default;

        virtual bool Add(int64_t key, SharedPtrNotNull<ILogger> logger) = 0;
        virtual auto Find(int64_t key) -> ILogger* = 0;
    };
}
