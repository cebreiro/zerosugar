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
        bool startFlushTask = false;
        PushTask(startFlushTask, function);

        if (startFlushTask)
        {
            StartFlushTask();
        }
    }

    void Strand::Post(std::move_only_function<void()> function)
    {
        bool startFlushTask = false;
        PushTask(startFlushTask, std::move(function));

        if (startFlushTask)
        {
            StartFlushTask();
        }
    }

    void Strand::Dispatch(const std::function<void()>& function)
    {
        if (CanExecuteImmediately())
        {
            std::invoke(function);
        }
        else
        {
            Post(function);
        }
    }

    void Strand::Dispatch(std::move_only_function<void()> function)
    {
        if (CanExecuteImmediately())
        {
            std::invoke(function);
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

    void Strand::PushTask(bool& startFlushTask, task_type task)
    {
        std::unique_lock lock(_mutex);

        _backBuffer.emplace_back(std::move(task));
        startFlushTask = std::exchange(_runningFlushTask, true) == false;
    }

    void Strand::StartFlushTask()
    {
        execution::Post(*_executor, [this, self = SharedFromThis()]()
            {
                this->FlushTasks();
            });
    }

    void Strand::FlushTasks()
    {
        SwapBuffer();

        {
            ExecutionContext::ExecutorGuard guard(this);

            ExecuteTasks();
        }

        if (!FinalizeFlush())
        {
            StartFlushTask();
        }
    }

    void Strand::SwapBuffer()
    {
        {
            std::unique_lock lock(_mutex);

            assert(_runningFlushTask);
            assert(!_backBuffer.empty());
            assert(_frontBuffer.empty());

            _frontBuffer.swap(_backBuffer);
        }
    }

    void Strand::ExecuteTasks()
    {
        for (auto& task : _frontBuffer)
        {
            std::visit([]<typename T>(T && va)
            {
                std::invoke(va);
            }, std::move(task));
        }

        _frontBuffer.clear();
    }

    bool Strand::FinalizeFlush()
    {
        std::unique_lock lock(_mutex);

        if (_backBuffer.empty())
        {
            _runningFlushTask = false;

            return true;
        }

        return false;
    }

    bool Strand::CanExecuteImmediately() const
    {
        return ExecutionContext::Contains(*this);
    }
}
