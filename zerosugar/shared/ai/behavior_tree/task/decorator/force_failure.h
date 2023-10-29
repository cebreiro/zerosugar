#pragma once
#include <cassert>
#include <stdexcept>
#include <format>
#include "zerosugar/shared/ai/behavior_tree/task/decorator/decorator.h"

namespace zerosugar::bt
{
    template <typename TContext>
    class ForceFailure : public DecoratorInheritanceHelper<ForceFailure<TContext>, TContext>
    {
    public:
        static constexpr const char* class_name = "force_failure";

    public:
        explicit ForceFailure(TContext& context);

        void Initialize(const pugi::xml_node& node) override;

    private:
        auto Run() const -> Runnable override;
    };

    template <typename TContext>
    ForceFailure<TContext>::ForceFailure(TContext& context)
        : DecoratorInheritanceHelper<ForceFailure, TContext>(context)
    {
    }

    template <typename TContext>
    void ForceFailure<TContext>::Initialize(const pugi::xml_node& node)
    {
        DecoratorInheritanceHelper<ForceFailure, TContext>::Initialize(node);
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
