#pragma once
#include <cassert>
#include "zerosugar/shared/ai/behavior_tree/task/decorator/decorator.h"
#include "zerosugar/shared/ai/behavior_tree/model/generated/task.proto.h"

namespace zerosugar::bt
{
    template <typename TContext>
    class RetryUntilSuccess : public Decorator<TContext, model::RetryUntilSuccess>
    {
    public:
        explicit RetryUntilSuccess(TContext& context);

    private:
        auto Run() const -> Runnable override;
    };

    template <typename TContext>
    RetryUntilSuccess<TContext>::RetryUntilSuccess(TContext& context)
        : Decorator<TContext, model::RetryUntilSuccess>(context)
    {
    }

    template <typename TContext>
    auto RetryUntilSuccess<TContext>::Run() const -> Runnable
    {
        for (size_t i = 0; i < this->_count;)
        {
            const State state = this->_task->Execute();
            switch (state)
            {
            case State::Success:
                co_return true;
            case State::Failure:
                ++i;
                continue;
            case State::Running:
                co_await running;
            case State::None:
            default:
                assert(false);
            }
        }
    }
}
