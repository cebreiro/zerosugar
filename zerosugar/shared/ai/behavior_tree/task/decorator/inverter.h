#pragma once
#include <cassert>
#include "zerosugar/shared/ai/behavior_tree/task/decorator/decorator.h"
#include "zerosugar/shared/ai/behavior_tree/model/generated/task.proto.h"

namespace zerosugar::bt
{
    template <typename TContext>
    class Inverter : public Decorator<TContext, model::Inverter>
    {
    public:
        explicit Inverter(TContext& context);

    private:
        auto Run() const -> Runnable override;
    };

    template <typename TContext>
    Inverter<TContext>::Inverter(TContext& context)
        : Decorator<TContext, model::Inverter>(context)
    {
    }

    template <typename TContext>
    auto Inverter<TContext>::Run() const -> Runnable
    {
        while (true)
        {
            const State state = this->_task->Execute();
            switch (state)
            {
            case State::Success:
                co_return false;
            case State::Failure:
                co_return true;
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
