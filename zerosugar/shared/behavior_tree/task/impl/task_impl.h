#pragma once
#include <vector>
#include "zerosugar/shared/behavior_tree/task/task.h"

namespace zerosugar::bt
{
    class Sequence : public TaskInheritanceHelper<Sequence>
    {
    public:
        static constexpr const char* class_name = "sequence";

    public:
        void Initialize(const TaskFactory& factory, const pugi::xml_node& node) override;

        void Reset() override;

    private:
        auto Run() const -> Runnable override;

    private:
        std::vector<task_pointer_type> _tasks;
    };

    class Selector : public TaskInheritanceHelper<Selector>
    {
    public:
        static constexpr const char* class_name = "selector";

    public:
        void Initialize(const TaskFactory& factory, const pugi::xml_node& node) override;

        void Reset() override;

    private:
        auto Run() const -> Runnable override;

    private:
        std::vector<task_pointer_type> _tasks;
    };

    // todo: select/select random + weight
}
