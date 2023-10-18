#pragma once
#include <vector>
#include <variant>
#include <functional>
#include <shared_mutex>
#include "zerosugar/shared/type/not_null_pointer.h"
#include "zerosugar/shared/execution/executor/executor_interface.h"

namespace zerosugar
{
    class Strand : public execution::IExecutor, public std::enable_shared_from_this<Strand>
    {
        using task_type = std::variant<std::function<void()>, std::move_only_function<void()>>;

    public:
        Strand() = delete;

        explicit Strand(SharedPtrNotNull<IExecutor> executor);

        void Stop() override;

        void Post(const std::function<void()>& function) override;
        void Post(std::move_only_function<void()> function) override;

        void Dispatch(const std::function<void()>& function) override;
        void Dispatch(std::move_only_function<void()> function) override;

        auto SharedFromThis() -> SharedPtrNotNull<IExecutor> override;
        auto SharedFromThis() const -> SharedPtrNotNull<const IExecutor> override;

    private:
        void PushTask(bool& startFlushTask, task_type task);
        void StartFlushTask();

        void FlushTasks();
        void SwapTasks();
        void ExecuteTasks();
        bool FinalizeFlush();

        static bool CanExecuteImmediately();

    private:
        SharedPtrNotNull<IExecutor> _executor;

        mutable std::shared_mutex _mutex;
        bool _runningFlushTask = false;
        std::vector<task_type> _frontBuffer;
        std::vector<task_type> _backBuffer;

        static thread_local bool _isInTaskExecuteContext;
    };
}
