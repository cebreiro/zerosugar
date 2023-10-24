#pragma once
#include <cassert>
#include <string>
#include <functional>
#include <unordered_map>
#include "zerosugar/shared/behavior_tree/task/task.h"

namespace zerosugar::bt
{
    class TaskFactory
    {
    public:
        virtual ~TaskFactory() = default;
        virtual auto CreateTask(const std::string& name) const -> task_pointer_type;

    private:
        struct Registry
        {
            Registry();

            template <task_concept T>
            void Register();

            std::unordered_map<std::string, std::function<task_pointer_type()>> factories;
        };

        static Registry _register;
    };

    template <task_concept T>
    void TaskFactory::Registry::Register()
    {
        [[maybe_unused]]
        const bool result = _register.factories.try_emplace(T::class_name, []() -> task_pointer_type
            {
                return std::make_unique<T>();
            }).second;
        assert(result);
    }
}
