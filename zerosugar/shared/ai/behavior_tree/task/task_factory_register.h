#pragma once
#include "zerosugar/shared/ai/behavior_tree/task/flow_control/sequence.h"
#include "zerosugar/shared/ai/behavior_tree/task/flow_control/selector.h"
#include "zerosugar/shared/ai/behavior_tree/task/decorator/force_failure.h"
#include "zerosugar/shared/ai/behavior_tree/task/decorator/force_success.h"
#include "zerosugar/shared/ai/behavior_tree/task/decorator/inverter.h"
#include "zerosugar/shared/ai/behavior_tree/task/decorator/repeat.h"
#include "zerosugar/shared/ai/behavior_tree/task/decorator/retry_until_success.h"

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
