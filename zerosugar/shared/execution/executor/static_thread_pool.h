#pragma once
#include <chrono>
#include "zerosugar/shared/execution/executor/executor_interface.h"

namespace zerosugar::execution
{
    class StaticThreadPool
    {
        StaticThreadPool();

    public:
        void Delay(const std::function<void()>& function, std::chrono::milliseconds milliseconds);
        void Delay(std::move_only_function<void()> function, std::chrono::milliseconds milliseconds);

        operator IExecutor& ();
        operator const IExecutor& () const;

        static auto GetInstance() -> StaticThreadPool&;

    private:
        SharedPtrNotNull<IExecutor> _executor;
    };
}
