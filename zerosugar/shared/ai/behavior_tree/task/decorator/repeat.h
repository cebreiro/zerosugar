#pragma once
#include <cassert>
#include "zerosugar/shared/ai/behavior_tree/task/decorator/decorator.h"
#include "zerosugar/shared/ai/behavior_tree/model/generated/task.proto.h"

namespace zerosugar::bt
{
    template <typename TContext>
    class Repeat : public Decorator<TContext, model::Repeat>
    {
    public:
        explicit Repeat(TContext& context);

    private:
        auto Run() const -> Runnable override;
    };

    template <typename TContext>
    Repeat<TContext>::Repeat(TContext& context)
        : Decorator<TContext, model::Repeat>(context)
    {
    }

    template <typename TContext>
    auto Repeat<TContext>::Run() const -> Runnable
    {
        for (size_t i = 0; i < this->_count;)
        {
            const State state = this->_task->Execute();
            switch (state)
            {
            case State::Success:
                ++i;
                break;
            case State::Failure:
                co_return false;
            case State::Running:
                co_await running;
                break;
            case State::None:
            default:
                assert(false);
            }
        }

        co_return true;
    }
}
