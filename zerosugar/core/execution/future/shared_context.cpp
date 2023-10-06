#include "shared_context.h"

#include <cassert>

namespace zerosugar::execution::future
{
    bool SharedContextBase::IsPending() const noexcept
    {
        return _status.load() == FutureStatus::Pending;
    }

    bool SharedContextBase::IsComplete() const noexcept
    {
        return _status.load() == FutureStatus::Complete;
    }

    bool SharedContextBase::IsCanceled() const noexcept
    {
        return _status.load() == FutureStatus::Canceled;
    }

    void SharedContextBase::Wait()
    {
        _status.wait(FutureStatus::Pending);
    }

    bool SharedContextBase::Cancel()
    {
        FutureStatus expected = FutureStatus::Pending;
        if (!_status.compare_exchange_strong(expected, FutureStatus::Canceled))
        {
            return false;
        }

        std::move_only_function<void()> continuation;
        {
            std::lock_guard lock(_mutex);
            _continuation.swap(continuation);
        }

        _status.notify_one();

        if (continuation)
        {
            continuation();
        }

        return true;
    }

    void SharedContextBase::OnFailure(const std::exception_ptr& exception)
    {
        FutureStatus expected = FutureStatus::Pending;
        if (!_status.compare_exchange_strong(expected, FutureStatus::Complete))
        {
            return;
        }

        std::move_only_function<void()> continuation;
        {
            std::lock_guard lock(_mutex);

            assert(!_exception);

            _exception = exception;
            _continuation.swap(continuation);
        }

        _status.notify_one();

        if (continuation)
        {
            continuation();
        }
    }

    void SharedContextBase::SetContinuation(std::move_only_function<void()> function)
    {
        if (_status.load() == FutureStatus::Pending)
        {
            std::lock_guard lock(_mutex);

            assert(!_continuation);
            _continuation = std::move(function);
        }

        if (function)
        {
            function();
        }
    }

    void SharedContextBase::SetExecutor(std::shared_ptr<IExecutor> executor) noexcept
    {
        assert(!_executor);

        _executor = std::move(executor);
    }

    auto SharedContextBase::GetExecutor() const -> IExecutor&
    {
        assert(_executor);

        return *_executor;
    }

    void SharedContext<void>::OnSuccess()
    {
        FutureStatus expected = FutureStatus::Pending;
        if (!_status.compare_exchange_strong(expected, FutureStatus::Complete))
        {
            return;
        }

        std::move_only_function<void()> continuation;
        {
            std::lock_guard lock(_mutex);

            _continuation.swap(continuation);
        }

        _status.notify_one();

        if (continuation)
        {
            continuation();
        }
    }

    void SharedContext<void>::Get()
    {
        switch (_status.load())
        {
        case FutureStatus::Pending:
        {
            Wait();
            Get();
        }
        break;
        case FutureStatus::Complete:
        {
            std::lock_guard lock(_mutex);
            if (_exception)
            {
                std::rethrow_exception(std::exchange(_exception, nullptr));
            }
        }
        break;
        case FutureStatus::Canceled:
        default:
            throw std::runtime_error("operation aborted");
        }
    }
}
