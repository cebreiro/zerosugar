#pragma once
#include <vector>
#include <variant>
#include <functional>
#include <optional>
#include <shared_mutex>
#include <tbb/spin_mutex.h>
#include <tbb/concurrent_queue.h>
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
        void Post(task_type task);
        void Dispatch(task_type task);

        void PostFlushTask();
        void FlushTasks();

    private:
        SharedPtrNotNull<IExecutor> _executor;

        tbb::spin_mutex _spinMutex;
        bool _posted = false;
        std::vector<std::thread::id> _owner;
        int32_t _taskCount = 0;

        tbb::concurrent_queue<task_type> _tasks;
    };
}
