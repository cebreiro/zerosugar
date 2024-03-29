#pragma once
#include <cassert>
#include "zerosugar/shared/ai/behavior_tree/task/decorator/decorator.h"
#include "zerosugar/shared/ai/behavior_tree/model/generated/task.proto.h"

namespace zerosugar::bt
{
    template <typename TContext>
    class ForceSuccess : public Decorator<TContext, model::ForceSuccess>
    {
    public:
        explicit ForceSuccess(TContext& context);

    private:
        auto Run() const -> Runnable override;
    };

    template <typename TContext>
    ForceSuccess<TContext>::ForceSuccess(TContext& context)
        : Decorator<TContext, model::ForceSuccess>(context)
    {
    }

    template <typename TContext>
    auto ForceSuccess<TContext>::Run() const -> Runnable
    {
        while (true)
        {
            const State state = this->_task->Execute();
            switch (state)
            {
            case State::Success:
            case State::Failure:
                co_return true;
            case State::Running:
                co_await running;
                break;
            case State::None:
            default:
                assert(false);
                co_return true;
            }
        }
    }
}
