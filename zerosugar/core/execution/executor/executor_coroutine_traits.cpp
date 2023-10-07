#include "executor_coroutine_traits.h"

#include "zerosugar/core/execution/context/execution_context.h"
#include "zerosugar/core/execution/executor/operation/post.h"

namespace zerosugar::execution
{
    ExecutorAwaiter::ExecutorAwaiter(SharedPtrNotNull<IExecutor> executor)
        : _executor(std::move(executor))
    {
    }

    bool ExecutorAwaiter::await_ready() const
    {
        const IExecutor* executor = ExecutionContext::GetExecutor();
        assert(executor);

        return _executor.get() == executor;
    }

    void ExecutorAwaiter::await_suspend(std::coroutine_handle<> handle)
    {
        Post(*_executor, [handle]()
            {
                handle();
            });
    }

    void ExecutorAwaiter::await_resume()
    {
    }

    auto operator co_await(IExecutor& executor) -> ExecutorAwaiter
    {
        return ExecutorAwaiter(executor.SharedFromThis());
    }
}
