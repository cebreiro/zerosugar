#pragma once
#include <concepts>
#include <ranges>
#include <type_traits>
#include "zerosugar/shared/execution/future/future.h"

namespace zerosugar
{
    template <typename... Futures>
    auto WaitAll(execution::IExecutor& executor, Futures&&... futures) -> Future<void>
    {
        static constexpr size_t operationCount = sizeof...(Futures);
        const auto counter = std::make_shared<std::atomic<size_t>>(0);

        auto context = (std::make_shared<future::SharedContext<void>>());
        context->SetExecutor(executor.SharedFromThis());

        auto fn = [=, &executor]<typename T>(Future<T>& fut) mutable
        {
            fut.ContinuationWith(executor, [=]([[maybe_unused]] Future<T>& self) mutable
                {
                    size_t current = counter->fetch_add(1) + 1;
                    if (current == operationCount)
                    {
                        context->OnSuccess();
                    }
                });
        };

        (..., fn(futures));

        return Future<void>(std::move(context));
    }

    template <std::ranges::range R>
    auto WaitAll(execution::IExecutor& executor, R&& range) -> Future<void>
    {
        const size_t operationCount = std::ranges::distance(range);
        const auto counter = std::make_shared<std::atomic<size_t>>(0);

        auto context = (std::make_shared<future::SharedContext<void>>());
        context->SetExecutor(executor.SharedFromThis());

        for (decltype(auto) fut : range)
        {
            fut.ContinuationWith(executor, [=]([[maybe_unused]] std::remove_cvref_t<decltype(fut)>& self) mutable
                {
                    size_t current = counter->fetch_add(1) + 1;
                    if (current == operationCount)
                    {
                        context->OnSuccess();
                    }
                });
        }

        return Future<void>(std::move(context));
    }
}
