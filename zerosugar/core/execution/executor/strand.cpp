#include "strand.h"

#include <algorithm>
#include <ranges>

#include "zerosugar/core/execution/executor/operation/post.h"

namespace zerosugar::execution
{
    thread_local bool Strand::_isInTaskExecuteContext = false;

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
        if (_isInTaskExecuteContext)
        {
            _backBuffer.emplace_back(function);
        }
        else
        {
            Post(function);
        }
    }

    void Strand::Dispatch(std::move_only_function<void()> function)
    {
        if (_isInTaskExecuteContext)
        {
            _backBuffer.emplace_back(std::move(function));
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

        _frontBuffer.emplace_back(std::move(task));
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
        SwapTasks();

        _isInTaskExecuteContext = true;

        ExecuteTasks();

        _isInTaskExecuteContext = false;

        if (!FinalizeFlush())
        {
            StartFlushTask();
        }
    }

    void Strand::SwapTasks()
    {
        {
            std::unique_lock lock(_mutex);

            assert(_runningFlushTask);
            assert(!_frontBuffer.empty());
            assert(_backBuffer.empty());

            _backBuffer.swap(_frontBuffer);
        }

        // for optimization
        std::ranges::reverse(_backBuffer);
    }

    void Strand::ExecuteTasks()
    {
        constexpr int64_t recursionCheckCount = 1'000'000;
        int64_t executeTaskCount = 0;

        while (!_backBuffer.empty())
        {
            task_type task = std::move(_backBuffer.back());
            _backBuffer.pop_back();

            std::visit([]<typename T>(T && va)
            {
                std::invoke(va);
            }, std::move(task));

            if (++executeTaskCount >= recursionCheckCount)
            {
                assert(false);
                break;
            }
        }
    }

    bool Strand::FinalizeFlush()
    {
        std::unique_lock lock(_mutex);

        if (_frontBuffer.empty())
        {
            _runningFlushTask = false;

            return true;
        }

        return false;
    }
}
