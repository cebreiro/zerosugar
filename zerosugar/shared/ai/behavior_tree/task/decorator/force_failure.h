#pragma once
#include <cassert>
#include <stdexcept>
#include <format>
#include "zerosugar/shared/ai/behavior_tree/task/decorator/decorator.h"
#include "zerosugar/shared/ai/behavior_tree/model/generated/task.proto.h"

namespace zerosugar::bt
{
    template <typename TContext>
    class ForceFailure : public Decorator<TContext, model::ForceFailure>
    {
    public:
        explicit ForceFailure(TContext& context);

    private:
        auto Run() const -> Runnable override;
    };

    template <typename TContext>
    ForceFailure<TContext>::ForceFailure(TContext& context)
        : Decorator<TContext, model::ForceFailure>(context)
    {
    }

    template <typename TContext>
    auto ForceFailure<TContext>::Run() const -> Runnable
    {
        while (true)
        {
            const State state = this->_task->Execute();
            switch (state)
            {
            case State::Success:
            case State::Failure:
                co_return false;
            case State::Running:
                co_await running;
                break;
            case State::None:
            default:
                assert(false);
                co_return false;
            }
        }
    }
}
