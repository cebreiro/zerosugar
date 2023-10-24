#include "decorator.h"

#include <stdexcept>
#include <format>

#include "zerosugar/shared/behavior_tree/task/task_factory.h"


namespace zerosugar::bt
{
    void Decorator::Initialize(const TaskFactory& factory, const pugi::xml_node& node)
    {
        if (node.children().empty())
        {
            throw std::runtime_error(
                std::format("fail to initialize {} node - no child. xml_node_name: {}",
                    this->GetName(), node.name()));
        }

        const pugi::xml_node& child = node.first_child();

        _task = factory.CreateTask(child.name());
        if (!_task)
        {
            throw std::runtime_error(
                std::format("fail to find {} node from factory. xml_node_name: {}",
                this->GetName(), node.name()));
        }

        _task->Initialize(factory, child);
    }

    void Decorator::Reset()
    {
        Task::Reset();

        _task->Reset();
    }
}
