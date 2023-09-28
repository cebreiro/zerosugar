#pragma once
#include <concepts>
#include <ranges>
#include <type_traits>
#include "zerosugar/core/execution/future/future.h"

namespace zerosugar::execution
{
    template <typename... Futures>
    auto WaitAny(IExecutor& executor, Futures&&... futures) -> Future<void>
    {
        const auto counter = std::make_shared<std::atomic<size_t>>(0);
        auto context = (std::make_shared<future::SharedContext<void>>());
        context->SetExecutor(executor.SharedFromThis());

        auto fn = [=, &executor]<typename T>(Future<T>&fut) mutable
        {
            fut.ContinuationWith(executor, [=]([[maybe_unused]] Future<T>& self) mutable
                {
                    size_t current = counter->fetch_add(1) + 1;
                    if (current == 1)
                    {
                        context->OnSuccess();
                    }
                });
        };

        (..., fn(futures));

        return Future<void>(std::move(context));
    }
}
