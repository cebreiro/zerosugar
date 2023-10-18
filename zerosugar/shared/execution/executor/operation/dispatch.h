#pragma once
#include <concepts>
#include <functional>
#include <type_traits>
#include "zerosugar/shared/execution/context/execution_context.h"
#include "zerosugar/shared/execution/executor/executor_interface.h"

namespace zerosugar::execution
{
    template <typename T, typename... Args> requires std::invocable<T, Args...>
    void Dispatch(IExecutor& executor, T&& function, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            using function_type = std::conditional_t<
                std::is_move_constructible_v<T>,
                std::move_only_function<void()>, std::function<void()>>;

            executor.Dispatch(function_type([fn = std::forward<T>(function), ex = executor.SharedFromThis()]() mutable
                {
                    ExecutionContext::ExecutorGuard guard(ex.get());
                    std::invoke(fn);
                }));
        }
        else
        {
            using function_type = std::conditional_t<
                std::is_move_constructible_v<T>&& std::conjunction_v<std::is_move_constructible<Args>...>,
                std::move_only_function<void()>, std::function<void()>>;

            executor.Dispatch(function_type(
                [fn = std::forward<T>(function), ...args = std::forward<Args>(args), ex = executor.SharedFromThis()]() mutable
                {
                    ExecutionContext::ExecutorGuard guard(ex.get());
                    std::invoke(fn, std::forward<Args>(args)...);
                }));
        }
    }
}
