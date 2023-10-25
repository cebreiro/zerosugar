#pragma once
#include "zerosugar/shared/behavior_tree/task/impl/task_impl.h"
#include "zerosugar/shared/behavior_tree/task/impl/decorator_impl.h"

namespace zerosugar::bt
{
    template <typename TContext>
    void RegisterBaseTaskToFactory()
    {
        auto& factory = TaskFactory<TContext>::GetInstance();

        factory.template Register<Sequence<TContext>>();
        factory.template Register<Selector<TContext>>();

        factory.template Register<Repeat<TContext>>();
        factory.template Register<RetryUntilSuccess<TContext>>();
        factory.template Register<Inverter<TContext>>();
        factory.template Register<ForceSuccess<TContext>>();
        factory.template Register<ForceFailure<TContext>>();
    }
};
