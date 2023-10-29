#pragma once
#include <cassert>
#include <string>
#include <functional>
#include <unordered_map>
#include "zerosugar/shared/ai/behavior_tree/task/task.h"

namespace zerosugar::bt
{
    template <typename TContext>
    class TaskFactory
    {
        TaskFactory() = default;
    public:
        static auto GetInstance() -> TaskFactory&;

        auto CreateTask(TContext& context, const std::string& name) const -> task_pointer_type<TContext>;

        template <typename T> requires task_concept<T, TContext>
        void Register();

        template <typename U> requires task_concept<U, TContext>
        class Registry
        {
        public:
            Registry();

            static Registry _registry;
        };

    private:
        std::unordered_map<std::string, std::function<task_pointer_type<TContext>(TContext&)>> _factories;
    };

    template <typename TContext>
    auto TaskFactory<TContext>::GetInstance() -> TaskFactory&
    {
        static TaskFactory instance;
        return instance;
    }

    template <typename TContext>
    auto TaskFactory<TContext>::CreateTask(TContext& context, const std::string& name) const -> task_pointer_type<TContext>
    {
        auto iter = _factories.find(name);
        return iter != _factories.end() ? iter->second(context) : nullptr;
    }

    template <typename TContext>
    template <typename T> requires task_concept<T, TContext>
    void TaskFactory<TContext>::Register()
    {
        [[maybe_unused]]
        const bool result = _factories.try_emplace(T::class_name, [](TContext& context) -> task_pointer_type<TContext>
            {
                return std::make_unique<T>(context);
            }).second;
        assert(result);
    }

    template <typename TContext>
    template <typename U> requires task_concept<U, TContext>
    TaskFactory<TContext>::Registry<U>::Registry()
    {
        TaskFactory<TContext>::GetInstance().template Register<U>();
    }

    template <typename TContext>
    template <typename U> requires task_concept<U, TContext>
    typename TaskFactory<TContext>::template Registry<U> TaskFactory<TContext>::Registry<U>::_registry;
}
