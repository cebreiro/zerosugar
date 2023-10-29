#pragma once
#include <cassert>
#include <stdexcept>
#include <format>
#include "zerosugar/shared/ai/behavior_tree/task/decorator/decorator.h"

namespace zerosugar::bt
{
    template <typename TContext>
    class ForceSuccess : public DecoratorInheritanceHelper<ForceSuccess<TContext>, TContext>
    {
    public:
        static constexpr const char* class_name = "force_success";

    public:
        explicit ForceSuccess(TContext& context);

        void Initialize(const pugi::xml_node& node) override;

    private:
        auto Run() const->Runnable override;
    };

    template <typename TContext>
    ForceSuccess<TContext>::ForceSuccess(TContext& context)
        : DecoratorInheritanceHelper<ForceSuccess, TContext>(context)
    {
    }

    template <typename TContext>
    void ForceSuccess<TContext>::Initialize(const pugi::xml_node& node)
    {
        DecoratorInheritanceHelper<ForceSuccess, TContext>::Initialize(node);
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
