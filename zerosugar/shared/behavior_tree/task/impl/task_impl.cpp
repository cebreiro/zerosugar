#include "task_impl.h"

#include <cassert>
#include <stdexcept>

#include "zerosugar/shared/behavior_tree/task/task_factory.h"

namespace zerosugar::bt
{
    void Sequence::Initialize(const TaskFactory& factory, const pugi::xml_node& node)
    {
        for (const pugi::xml_node& child : node.children())
        {
            const auto* childName = child.name();
            auto task = factory.CreateTask(childName);
            if (!task)
            {
                throw std::runtime_error("fail to find sequence child name");
            }

            _tasks.emplace_back(std::move(task))->Initialize(factory, child);
        }
    }

    void Sequence::Reset()
    {
        Task::Reset();

        for (const auto& task : _tasks)
        {
            task->Reset();
        }
    }

    auto Sequence::Run() const -> Runnable
    {
        const int64_t size = std::ssize(_tasks);
        for (int64_t i = 0; i < size;)
        {
            auto& task = _tasks[i];

            switch (task->Execute())
            {
            case State::Success:
                ++i;
                continue;
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

    void Selector::Initialize(const TaskFactory& factory, const pugi::xml_node& node)
    {
        for (const pugi::xml_node& child : node.children())
        {
            auto task = factory.CreateTask(child.name());
            if (!task)
            {
                throw std::runtime_error("fail to find selector child name");
            }

            _tasks.emplace_back(std::move(task))->Initialize(factory, child);
        }
    }

    void Selector::Reset()
    {
        Task::Reset();

        for (const auto& task : _tasks)
        {
            task->Reset();
        }
    }

    auto Selector::Run() const -> Runnable
    {
        const int64_t size = std::ssize(_tasks);
        for (int64_t i = 0; i < size;)
        {
            auto& task = _tasks[i];

            switch (task->Execute())
            {
            case State::Success:
                co_return true;
            case State::Failure:
                ++i;
                continue;
            case State::Running:
                co_await running;
                break;
            case State::None:
            default:
                assert(false);
            }
        }

        co_return false;
    }
}
