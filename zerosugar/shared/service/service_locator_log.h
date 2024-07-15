#pragma once
#include <string>
#include <source_location>
#include <fmt/format.h>
#include "zerosugar/shared/app/app.h"
#include "zerosugar/shared/log/log_service.h"


#define ZEROSUGAR_LOG_IMPL(locator, logLevel, message) \
if (ILogService* logService = locator.Find<ILogService>(); logService != nullptr) \
    logService->Log(logLevel, message, std::source_location::current())


#define ZEROSUGAR_LOG_DEBUG(locator, message)  ZEROSUGAR_LOG_IMPL(locator, zerosugar::LogLevel::Debug, message)
#define ZEROSUGAR_LOG_INFO(locator, message)  ZEROSUGAR_LOG_IMPL(locator, zerosugar::LogLevel::Info, message)
#define ZEROSUGAR_LOG_WARN(locator, message)  ZEROSUGAR_LOG_IMPL(locator, zerosugar::LogLevel::Warn, message)
#define ZEROSUGAR_LOG_ERROR(locator, message)  ZEROSUGAR_LOG_IMPL(locator, zerosugar::LogLevel::Error, message)
#define ZEROSUGAR_LOG_CRITICAL(locator, message)  ZEROSUGAR_LOG_IMPL(locator, zerosugar::LogLevel::Critical, message)
