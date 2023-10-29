#pragma once
#include <vector>
#include <stdexcept>
#include "zerosugar/shared/behavior_tree/task/task.h"
#include "zerosugar/shared/behavior_tree/task/task_factory.h"

namespace zerosugar::bt
{
   template <typename TContext>
    class Selector : public TaskInheritanceHelper<Selector<TContext>, TContext>
    {
    public:
        static constexpr const char* class_name = "selector";

    public:
        explicit Selector(TContext& context);
        void Initialize(const pugi::xml_node& node) override;

        void Reset() override;

    private:
        auto Run() const -> Runnable override;

    private:
        std::vector<task_pointer_type<TContext>> _tasks;
    };

    template <typename TContext>
    Selector<TContext>::Selector(TContext& context)
        : TaskInheritanceHelper<Selector, TContext>(context)
    {
    }

    template <typename TContext>
    void Selector<TContext>::Initialize(const pugi::xml_node& node)
    {
        auto& factory = TaskFactory<TContext>::GetInstance();

        for (const pugi::xml_node& child : node.children())
        {
            auto task = factory.CreateTask(this->_context, child.name());
            if (!task)
            {
                throw std::runtime_error("fail to find selector child name");
            }

            _tasks.emplace_back(std::move(task))->Initialize(child);
        }
    }

    template <typename TContext>
    void Selector<TContext>::Reset()
    {
        TaskInheritanceHelper<Selector, TContext>::Reset();
        for (const auto& task : _tasks)
        {
            task->Reset();
        }
    }

    template <typename TContext>
    auto Selector<TContext>::Run() const -> Runnable
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
