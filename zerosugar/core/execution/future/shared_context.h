#pragma once
#include <concepts>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <optional>
#include "zerosugar/core/execution/context/cancelable_interface.h"
#include "zerosugar/core/execution/future/future_status.h"

namespace zerosugar::execution
{
    class IExecutor;
}

namespace zerosugar::execution::future
{
    class SharedContextBase : public ICancelable
    {
    public:
        SharedContextBase(const SharedContextBase&) = delete;
        SharedContextBase(SharedContextBase&&) noexcept = delete;
        SharedContextBase& operator=(const SharedContextBase&) = delete;
        SharedContextBase& operator=(SharedContextBase&&) noexcept = delete;

        SharedContextBase() = default;
        ~SharedContextBase() override = default;

        bool IsPending() const noexcept;
        bool IsComplete() const noexcept;
        bool IsCanceled() const noexcept final;

        void Wait();
        bool WaitFor(std::chrono::milliseconds milliseconds);

        bool Cancel() final;
        void OnFailure(const std::exception_ptr& exception);

        void SetContinuation(std::move_only_function<void()> function);
        void SetExecutor(std::shared_ptr<IExecutor> executor) noexcept;

        auto GetExecutor() const -> IExecutor&;

    protected:
        std::mutex _mutex;
        std::condition_variable _condVariable;
        std::move_only_function<void()> _continuation = {};
        std::exception_ptr _exception = nullptr;
        std::atomic<FutureStatus> _status = FutureStatus::Pending;
        std::shared_ptr<IExecutor> _executor;
    };

    template <typename T>
    class SharedContext;

    template <>
    class SharedContext<void> : public SharedContextBase
    {
    public:
        void OnSuccess();
        void Get();
    };

    template <std::move_constructible T>
    class SharedContext<T> : public SharedContextBase
    {
    public:
        void OnSuccess(T&& value);
        void OnSuccess(const T& value);

        auto Get() -> T;

    private:
        std::optional<T> _value = std::nullopt;
    };

    template <std::move_constructible T>
    void SharedContext<T>::OnSuccess(T&& value)
    {
        std::move_only_function<void()> continuation;
        {
            std::lock_guard lock(_mutex);

            FutureStatus expected = FutureStatus::Pending;
            if (!_status.compare_exchange_strong(expected, FutureStatus::Complete))
            {
                return;
            }

            _value = std::move(value);
            _continuation.swap(continuation);
        }

        _condVariable.notify_one();

        if (continuation)
        {
            continuation();
        }
    }

    template <std::move_constructible T>
    void SharedContext<T>::OnSuccess(const T& value)
    {
        T temp(value);

        OnSuccess(std::move(temp));
    }

    template <std::move_constructible T>
    auto SharedContext<T>::Get() -> T
    {
        switch (_status.load())
        {
        case FutureStatus::Pending:
        {
            Wait();
            return Get();
        }
        case FutureStatus::Complete:
        {
            std::optional<T> value = std::nullopt;
            {
                std::lock_guard lock(_mutex);
                if (_exception)
                {
                    std::rethrow_exception(std::exchange(_exception, nullptr));
                }

                if (_value.has_value())
                {
                    _value.swap(value);
                }
            }

            if (!value.has_value())
            {
                throw std::runtime_error("value is already used");
            }

            return std::move(value.value());
        }
        case FutureStatus::Canceled:
        default:
            throw std::runtime_error("operation aborted");
        }
    }
}
