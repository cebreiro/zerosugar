#pragma once
#include <cassert>
#include <stdexcept>
#include <format>
#include "zerosugar/shared/behavior_tree/task/decorator.h"

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

    template <typename TContext>
    class ForceSuccess : public DecoratorInheritanceHelper<ForceSuccess<TContext>, TContext>
    {
    public:
        static constexpr const char* class_name = "force_success";

    public:
        explicit ForceSuccess(TContext& context);

        void Initialize(const pugi::xml_node& node) override;

    private:
        auto Run() const -> Runnable override;
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
