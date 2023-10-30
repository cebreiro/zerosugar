#pragma once
#include <vector>
#include "zerosugar/shared/ai/behavior_tree/task/task.h"
#include "zerosugar/shared/ai/behavior_tree/task/task_factory.h"
#include "zerosugar/shared/ai/behavior_tree/model/generated/task.proto.h"

namespace zerosugar::bt
{
    template <typename TContext>
    class Sequence : public TaskT<TContext, model::Sequence>
    {
    public:
        explicit Sequence(TContext& context);

        bool Initialize(const pugi::xml_node& node) override;

        void Reset() override;

    private:
        auto Run() const -> Runnable override;

    private:
        std::vector<task_pointer_type<TContext>> _tasks;
    };

    template <typename TContext>
    Sequence<TContext>::Sequence(TContext& context)
        : TaskT<TContext, model::Sequence>(context)
    {
    }

    template <typename TContext>
    bool Sequence<TContext>::Initialize(const pugi::xml_node& node)
    {
        if (!TaskT<TContext, model::Sequence>::Initialize(node))
        {
            return false;
        }

        auto& factory = TaskFactory<TContext>::GetInstance();

        for (const pugi::xml_node& child : node.children())
        {
            auto task = factory.CreateTask(this->_context, child.name());
            if (!task)
            {
                return false;
            }

            if (!task->Initialize(child))
            {
                return false;
            }

            _tasks.emplace_back(std::move(task));
        }

        return true;
    }

    template <typename TContext>
    void Sequence<TContext>::Reset()
    {
        for (const auto& task : _tasks)
        {
            task->Reset();
        }
    }

    template <typename TContext>
    auto Sequence<TContext>::Run() const -> Runnable
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
}
