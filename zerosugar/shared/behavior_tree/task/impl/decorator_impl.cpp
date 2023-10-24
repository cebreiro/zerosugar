#include "decorator_impl.h"

#include <cassert>
#include <stdexcept>
#include <format>

#include <zerosugar/shared/behavior_tree/task/task_factory.h>

namespace zerosugar::bt
{
    void Repeat::Initialize(const TaskFactory& factory, const pugi::xml_node& node)
    {
        Decorator::Initialize(factory, node);

        if (const pugi::xml_attribute& attribute = node.attribute("count"); attribute)
        {
            _count = attribute.as_uint();
        }
        else
        {
            throw std::runtime_error(
                std::format("fail to find attribute. node: {}, xml_node_name: {}, attribute: {}",
                    GetName(), node.name(), "count"));
        }
    }

    auto Repeat::Run() const -> Runnable
    {
        for (size_t i = 0; i < _count;)
        {
            const State state = _task->Execute();
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

    void RetryUntilSuccess::Initialize(const TaskFactory& factory, const pugi::xml_node& node)
    {
        Decorator::Initialize(factory, node);

        if (const pugi::xml_attribute& attribute = node.attribute("count"); attribute)
        {
            _count = attribute.as_uint();
        }
        else
        {
            throw std::runtime_error(
                std::format("fail to find attribute. node: {}, xml_node_name: {}, attribute: {}",
                    GetName(), node.name(), "count"));
        }
    }

    auto RetryUntilSuccess::Run() const -> Runnable
    {
        for (size_t i = 0; i < _count;)
        {
            const State state = _task->Execute();
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

    void Inverter::Initialize(const TaskFactory& factory, const pugi::xml_node& node)
    {
        Decorator::Initialize(factory, node);
    }

    auto Inverter::Run() const -> Runnable
    {
        while (true)
        {
            const State state = _task->Execute();
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

    void ForceSuccess::Initialize(const TaskFactory& factory, const pugi::xml_node& node)
    {
        Decorator::Initialize(factory, node);
    }

    auto ForceSuccess::Run() const -> Runnable
    {
        while (true)
        {
            const State state = _task->Execute();
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

    void ForceFailure::Initialize(const TaskFactory& factory, const pugi::xml_node& node)
    {
        Decorator::Initialize(factory, node);
    }

    auto ForceFailure::Run() const -> Runnable
    {
        while (true)
        {
            const State state = _task->Execute();
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
