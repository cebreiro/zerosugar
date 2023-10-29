#pragma once
#include <stdexcept>
#include <format>
#include "zerosugar/shared/ai/behavior_tree/task/task.h"
#include "zerosugar/shared/ai/behavior_tree/task/task_factory.h"

namespace zerosugar::bt
{
    template <typename TContext>
    class Decorator : public Task<TContext>
    {
    public:
        explicit Decorator(TContext& context);

        void Initialize(const pugi::xml_node& node) override;

        void Reset() override;

    protected:
        task_pointer_type<TContext> _task;
    };


    template <typename TContext>
    Decorator<TContext>::Decorator(TContext& context)
        : Task<TContext>(context)
    {
    }

    template <typename TContext>
    void Decorator<TContext>::Initialize(const pugi::xml_node& node)
    {
        auto& factory = TaskFactory<TContext>::GetInstance();

        if (node.children().empty())
        {
            throw std::runtime_error(
                std::format("fail to initialize {} node - no child. xml_node_name: {}",
                    this->GetName(), node.name()));
        }

        const pugi::xml_node& child = node.first_child();

        _task = factory.CreateTask(this->_context, child.name());
        if (!_task)
        {
            throw std::runtime_error(
                std::format("fail to find {} node from factory. xml_node_name: {}",
                this->GetName(), node.name()));
        }

        _task->Initialize(child);
    }

    template <typename TContext>
    void Decorator<TContext>::Reset()
    {
        Task<TContext>::Reset();

        _task->Reset();
    }

    template <typename T, typename TContext>
    class DecoratorInheritanceHelper : public Decorator<TContext>
    {
    public:
        explicit DecoratorInheritanceHelper(TContext& context);

        auto GetName() const -> std::string_view override;
    };

    template <typename T, typename TContext>
    DecoratorInheritanceHelper<T, TContext>::DecoratorInheritanceHelper(TContext& context)
        : Decorator<TContext>(context)
    {
    }

    template <typename T, typename TContext>
    auto DecoratorInheritanceHelper<T, TContext>::GetName() const -> std::string_view
    {
        return T::class_name;
    }
}
