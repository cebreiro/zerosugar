#pragma once
#include <cassert>
#include <stdexcept>
#include <format>
#include "zerosugar/shared/behavior_tree/task/decorator/decorator.h"

namespace zerosugar::bt
{
    template <typename TContext>
    class Inverter : public DecoratorInheritanceHelper<Inverter<TContext>, TContext>
    {
    public:
        static constexpr const char* class_name = "inverter";

    public:
        explicit Inverter(TContext& context);

        void Initialize(const pugi::xml_node& node) override;

    private:
        auto Run() const -> Runnable override;
    };

    template <typename TContext>
    Inverter<TContext>::Inverter(TContext& context)
        : DecoratorInheritanceHelper<Inverter, TContext>(context)
    {
    }

    template <typename TContext>
    void Inverter<TContext>::Initialize(const pugi::xml_node& node)
    {
        DecoratorInheritanceHelper<Inverter, TContext>::Initialize(node);
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
