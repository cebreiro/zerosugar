#pragma once
#include <cassert>
#include <stdexcept>
#include <format>
#include "zerosugar/shared/behavior_tree/task/decorator/decorator.h"

namespace zerosugar::bt
{
    template <typename TContext>
    class RetryUntilSuccess : public DecoratorInheritanceHelper<RetryUntilSuccess<TContext>, TContext>
    {
    public:
        static constexpr const char* class_name = "retry_until_success";

    public:
        explicit RetryUntilSuccess(TContext& context);

        void Initialize(const pugi::xml_node& node) override;

    private:
        auto Run() const -> Runnable override;

    private:
        size_t _count = 0;
    };

    template <typename TContext>
    RetryUntilSuccess<TContext>::RetryUntilSuccess(TContext& context)
        : DecoratorInheritanceHelper<RetryUntilSuccess, TContext>(context)
    {
    }

    template <typename TContext>
    void RetryUntilSuccess<TContext>::Initialize(const pugi::xml_node& node)
    {
        DecoratorInheritanceHelper<RetryUntilSuccess, TContext>::Initialize(node);

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
    auto RetryUntilSuccess<TContext>::Run() const -> Runnable
    {
        for (size_t i = 0; i < _count;)
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
