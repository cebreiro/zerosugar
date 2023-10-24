#pragma once
#include "zerosugar/shared/behavior_tree/task/state.h"
#include "zerosugar/shared/behavior_tree/task/task.h"

namespace zerosugar
{
    namespace bt
    {
        class TaskFactory;
    }

    class BehaviorTree
    {
    public:
        BehaviorTree() = default;
        void Initialize(const bt::TaskFactory& factory, const pugi::xml_node& root);

        bool IsValid() const;
        auto Execute() -> bt::State;

        void Reset();

        auto GetState() const -> bt::State;

    private:
        bt::task_pointer_type _root = nullptr;
    };
}
