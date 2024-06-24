#include "strand.h"

#include <cassert>
#include <algorithm>
#include <ranges>

#include "zerosugar/shared/execution/executor/operation/post.h"

namespace zerosugar
{
    Strand::Strand(SharedPtrNotNull<IExecutor> executor)
        : _executor(std::move(executor))
    {
    }

    void Strand::Stop()
    {
    }

    void Strand::Post(const std::function<void()>& function)
    {
        bool shouldPostTask = false;
        {
            std::lock_guard lock(_spinMutex);

            ++_taskCount;
            shouldPostTask = std::exchange(_posted, true) == false;
        }

        _tasks.emplace(function);

        if (shouldPostTask)
        {
            PostFlushTask();
        }
    }

    void Strand::Post(std::move_only_function<void()> function)
    {
        bool shouldPostTask = false;
        {
            std::lock_guard lock(_spinMutex);

            ++_taskCount;
            shouldPostTask = std::exchange(_posted, true) == false;
        }

        _tasks.emplace(std::move(function));

        if (shouldPostTask)
        {
            PostFlushTask();
        }
    }

    void Strand::Dispatch(const std::function<void()>& function)
    {
        const auto threadId = std::this_thread::get_id();
        bool shouldInvokeImmediately = false;
        {
            std::lock_guard lock(_spinMutex);

            if (_owner.has_value() && *_owner == threadId)
            {
                shouldInvokeImmediately = true;
            }
            else if (!_owner.has_value())
            {
                _owner = threadId;

                shouldInvokeImmediately = true;
            }

            if (!shouldInvokeImmediately)
            {
                ++_taskCount;
            }
        }

        if (shouldInvokeImmediately)
        {
            ExecutionContext::ExecutorGuard guard(this);

            function();
        }
        else
        {
            Post(function);
        }
    }

    void Strand::Dispatch(std::move_only_function<void()> function)
    {
        const auto threadId = std::this_thread::get_id();

        bool shouldInvokeImmediately = false;
        {
            std::lock_guard lock(_spinMutex);

            if (_owner.has_value() && *_owner == threadId)
            {
                shouldInvokeImmediately = true;
            }
            else if (!_owner.has_value())
            {
                _owner = threadId;

                shouldInvokeImmediately = true;
            }

            if (!shouldInvokeImmediately)
            {
                ++_taskCount;
            }
        }

        if (shouldInvokeImmediately)
        {
            ExecutionContext::ExecutorGuard guard(this);

            function();
        }
        else
        {
            Post(std::move(function));
        }
    }

    auto Strand::SharedFromThis() -> SharedPtrNotNull<IExecutor>
    {
        return shared_from_this();
    }

    auto Strand::SharedFromThis() const -> SharedPtrNotNull<const IExecutor>
    {
        return shared_from_this();
    }

    void Strand::PostFlushTask()
    {
        execution::Post(*_executor, [self = SharedFromThis(), this]()
            {
                this->FlushTasks();
            });
    }

    void Strand::FlushTasks()
    {
        const auto threadId = std::this_thread::get_id();
        int32_t taskCount = 0;
        {
            std::lock_guard lock(_spinMutex);

            if (_owner.has_value())
            {
                if (*_owner != threadId)
                {
                    return;
                }
            }
            else
            {
                _owner = threadId;
            }

            taskCount = _taskCount;
        }

        int32_t count = 0;

        while (count < taskCount)
        {
            task_type task;
            if (_tasks.try_pop(task))
            {
                ++count;

                std::visit([]<typename T>(T && va)
                {
                    va();
                }, std::move(task));
            }
        }

        bool shouldContinue = false;
        {
            std::lock_guard lock(_spinMutex);

            _owner.reset();

            _taskCount -= taskCount;
            shouldContinue = _taskCount > 0;

            if (!shouldContinue)
            {
                _posted = false;
            }
        }

        if (shouldContinue)
        {
            PostFlushTask();
        }
    }
}
