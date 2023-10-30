#pragma once
#include <mutex>
#include <cassert>
#include <stdexcept>
#include "zerosugar/shared/ai/behavior_tree/task/state.h"
#include "zerosugar/shared/ai/behavior_tree/task/task.h"
#include "zerosugar/shared/ai/behavior_tree/task/task_factory.h"
#include "zerosugar/shared/ai/behavior_tree/task/task_factory_register.h"

namespace zerosugar
{
    template <typename TContext>
    class BehaviorTree
    {
    public:
        BehaviorTree() = default;
        explicit  BehaviorTree(std::remove_cvref_t<TContext>& context);

        bool Initialize(const pugi::xml_node& root);

        bool IsValid() const;
        auto Execute() -> bt::State;

        void Reset();

        auto GetState() const -> bt::State;

    private:
        std::remove_const_t<TContext>* _context = nullptr;
        bt::task_pointer_type<TContext> _root = nullptr;

        static std::once_flag _registerFlag;
    };

    template <typename TContext>
    BehaviorTree<TContext>::BehaviorTree(std::remove_cvref_t<TContext>& context)
        : _context(&context)
    {
    }

    template <typename TContext>
    bool BehaviorTree<TContext>::Initialize(const pugi::xml_node& root)
    {
        std::call_once(_registerFlag, bt::RegisterBaseTaskToFactory<TContext>);

        const auto& child = root.first_child();
        if (!child)
        {
            return false;
        }

        auto& factory = bt::TaskFactory<TContext>::GetInstance();

        _root = factory.CreateTask(*_context, child.name());
        if (!_root)
        {
            return false;
        }

        return _root->Initialize(child);
    }

    template <typename TContext>
    bool BehaviorTree<TContext>::IsValid() const
    {
        return _root.operator bool();
    }

    template <typename TContext>
    auto BehaviorTree<TContext>::Execute() -> bt::State
    {
        assert(IsValid());
        return _root->Execute();
    }

    template <typename TContext>
    void BehaviorTree<TContext>::Reset()
    {
        assert(IsValid());

        _root->Reset();
    }

    template <typename TContext>
    auto BehaviorTree<TContext>::GetState() const -> bt::State
    {
        assert(IsValid());

        return _root->GetState();
    }

    template <typename TContext>
    std::once_flag BehaviorTree<TContext>::_registerFlag;
}
