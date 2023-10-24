#pragma once
#include "zerosugar/shared/behavior_tree/task/task.h"

namespace zerosugar::bt
{
    class Decorator : public Task
    {
    public:
        void Initialize(const TaskFactory& factory, const pugi::xml_node& node) override;

        void Reset() override;

    protected:
        task_pointer_type _task;
    };


    template <typename T>
    class DecoratorInheritanceHelper : public Decorator
    {
    public:
        auto GetName() const -> std::string_view override {
            return T::class_name;
        }
    };
}
