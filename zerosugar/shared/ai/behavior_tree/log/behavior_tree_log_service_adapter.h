#pragma once
#include "zerosugar/shared/ai/behavior_tree/log/behavior_tree_logger_interface.h"
#include "zerosugar/shared/log/log_level.h"
#include "zerosugar/shared/type/not_null_pointer.h"

namespace zerosugar
{
    class ILogService;
}

namespace zerosugar
{
    class BehaviorTreeLogServiceAdapter final : public IBehaviorTreeLogger
    {
    public:
        BehaviorTreeLogServiceAdapter() = delete;
        BehaviorTreeLogServiceAdapter(ILogService& logService, LogLevel logLevel);

        void LogBehaviorTreeStart(const std::string& btName) override;
        void LogBehaviorTreeEnd(const std::string& btName, bool success) override;
        void LogBehaviorTreeFinalize(const std::string& btName) override;

        void LogNodeStart(const std::string& btName, const std::string& nodeName) override;
        void LogNodeEnd(const std::string& btName, const std::string& nodeName, bool success) override;
        void LogNodeSuspend(const std::string& btName, const std::string& nodeName) override;
        void LogNodeResumeEvent(const std::string& btName, const std::string& nodeName, const std::string& event) override;

    private:
        ILogService& _logService;
        LogLevel _logLevel = LogLevel::Debug;
    };
}
