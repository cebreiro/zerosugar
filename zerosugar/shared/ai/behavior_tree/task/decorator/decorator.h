#pragma once
#include <stdexcept>
#include <format>
#include "zerosugar/shared/ai/behavior_tree/task/task.h"
#include "zerosugar/shared/ai/behavior_tree/task/task_factory.h"

namespace zerosugar::bt
{
    template <typename TContext, typename TModel>
    class Decorator : public TaskT<TContext, TModel>
    {
    public:
        explicit Decorator(TContext& context);

        bool Initialize(const pugi::xml_node& node) override;

        void Reset() override;

    protected:
        task_pointer_type<TContext> _task;
    };


    template <typename TContext, typename TModel>
    Decorator<TContext, TModel>::Decorator(TContext& context)
        : TaskT<TContext, TModel>(context)
    {
    }

    template <typename TContext, typename TModel>
    bool Decorator<TContext, TModel>::Initialize(const pugi::xml_node& node)
    {
        if (!TaskT<TContext, TModel>::Initialize(node))
        {
            return false;
        }

        const pugi::xml_node& child = node.first_child();
        if (!child)
        {
            return false;
        }

        _task = TaskFactory<TContext>::GetInstance().CreateTask(this->_context, child.name());
        if (!_task)
        {
            return false;
        }

        return _task->Initialize(child);
    }

    template <typename TContext, typename TModel>
    void Decorator<TContext, TModel>::Reset()
    {
        _task->Reset();
    }
}
