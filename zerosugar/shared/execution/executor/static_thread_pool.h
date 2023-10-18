#pragma once
#include <chrono>
#include "zerosugar/shared/execution/executor/executor_interface.h"

namespace zerosugar
{
    class StaticThreadPool
    {
        StaticThreadPool();

    public:
        void Delay(const std::function<void()>& function, std::chrono::milliseconds milliseconds);
        void Delay(std::move_only_function<void()> function, std::chrono::milliseconds milliseconds);

        operator execution::IExecutor& ();
        operator const execution::IExecutor& () const;

        static auto GetInstance() -> StaticThreadPool&;

    private:
        SharedPtrNotNull<execution::IExecutor> _executor;
    };
}
