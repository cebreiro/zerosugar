#include "behavior_tree.h"

#include <cassert>
#include <stdexcept>

#include "zerosugar/shared/behavior_tree/task/task_factory.h"

namespace zerosugar
{
    void BehaviorTree::Initialize(const bt::TaskFactory& factory, const pugi::xml_node& root)
    {
        const auto& child = root.first_child();
        if (!child)
        {
            throw std::runtime_error("empty behavior tree");
        }

        _root = factory.CreateTask(root.first_child().name());
        _root->Initialize(factory, root.first_child());
    }

    bool BehaviorTree::IsValid() const
    {
        return _root.operator bool();
    }

    auto BehaviorTree::Execute() -> bt::State
    {
        assert(IsValid());
        return _root->Execute();
    }

    void BehaviorTree::Reset()
    {
        assert(IsValid());
        _root->Reset();
    }

    auto BehaviorTree::GetState() const -> bt::State
    {
        assert(IsValid());
        return _root->GetState();
    }
}
