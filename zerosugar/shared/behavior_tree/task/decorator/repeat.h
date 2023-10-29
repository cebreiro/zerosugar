#pragma once
#include <cassert>
#include <stdexcept>
#include <format>
#include "zerosugar/shared/behavior_tree/task/decorator/decorator.h"

namespace zerosugar::bt
{
    template <typename TContext>
    class Repeat : public DecoratorInheritanceHelper<Repeat<TContext>, TContext>
    {
    public:
        static constexpr const char* class_name = "repeat";

    public:
        explicit Repeat(TContext& context);

        void Initialize(const pugi::xml_node& node) override;

    private:
        auto Run() const -> Runnable override;

    private:
        size_t _count = 0;
    };

    template <typename TContext>
    Repeat<TContext>::Repeat(TContext& context)
        : DecoratorInheritanceHelper<Repeat, TContext>(context)
    {
    }

    template <typename TContext>
    void Repeat<TContext>::Initialize(const pugi::xml_node& node)
    {
        Decorator<TContext>::Initialize(node);

        if (const pugi::xml_attribute& attribute = node.attribute("count"); attribute)
        {
            _count = attribute.as_uint();
        }
        else
        {
            throw std::runtime_error(
                std::format("fail to find attribute. node: {}, xml_node_name: {}, attribute: {}",
                    this->GetName(), node.name(), "count"));
        }
    }

    template <typename TContext>
    auto Repeat<TContext>::Run() const -> Runnable
    {
        for (size_t i = 0; i < _count;)
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
