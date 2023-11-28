#pragma once
#include <string>
#include <format>
#include <source_location>
#include "zerosugar/shared/app/app.h"
#include "zerosugar/shared/log/log_service.h"


#define ZEROSUGAR_APP_LOG(logLevel, message) \
if (ILogService* logService = App::Find<ILogService>(); logService != nullptr) \
    logService->Log(logLevel, message, std::source_location::current())


#define ZEROSUGAR_APP_LOG_DEBUG(message)  ZEROSUGAR_APP_LOG(zerosugar::LogLevel::Debug, message)
#define ZEROSUGAR_APP_LOG_INFO(message)  ZEROSUGAR_APP_LOG(zerosugar::LogLevel::Info, message)
#define ZEROSUGAR_APP_LOG_WAN(message)  ZEROSUGAR_APP_LOG(zerosugar::LogLevel::Warn, message)
#define ZEROSUGAR_APP_LOG_ERROR(message)  ZEROSUGAR_APP_LOG(zerosugar::LogLevel::Error, message)
#define ZEROSUGAR_APP_LOG_CRITICAL(message)  ZEROSUGAR_APP_LOG(zerosugar::LogLevel::Critical, message)
