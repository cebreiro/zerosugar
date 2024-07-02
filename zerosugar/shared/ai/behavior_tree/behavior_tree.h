#pragma once
#include <cassert>
#include <boost/unordered/unordered_flat_set.hpp>
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"
#include "zerosugar/shared/ai/behavior_tree/node/bt_node_state.h"
#include "zerosugar/shared/ai/behavior_tree/node/bt_node_event.h"

namespace zerosugar::bt
{
    class BlackBoard;
}

namespace zerosugar
{
    class BehaviorTree
    {
    public:
        BehaviorTree() = delete;
        BehaviorTree(bt::BlackBoard& blackBoard);
        ~BehaviorTree();

        void Initialize(std::vector<UniquePtrNotNull<bt::INode>> nodes);
        void Finalize();

        bool IsRunning() const;

        void RunOnce();

        template <bt::bt_event_concept E>
        void Notify(const E& e);

        auto GetBlackBoard() -> bt::BlackBoard&;
        auto GetBlackBoard() const -> const bt::BlackBoard&;

    private:
        void Traverse();

        void FinalizeTraverse();

    private:
        bt::BlackBoard& _blackBoard;

        std::vector<UniquePtrNotNull<bt::INode>> _storage;
        bt::NodePtr _rootNode;
        bt::node::State _currentState = bt::node::State::Success;

        std::vector<bt::NodePtr> _stack;
        boost::unordered::unordered_flat_set<bt::INode*> _visited;
        std::coroutine_handle<bt::node::Result::promise_type> _runningNodeCoroutine;
    };

    template <bt::bt_event_concept E>
    void BehaviorTree::Notify(const E& e)
    {
        if (_currentState != bt::node::State::Running)
        {
            return;
        }

        assert(_runningNodeCoroutine);

        if (bt::node::Result::promise_type& promise = _runningNodeCoroutine.promise();
            promise.IsWaitingFor(typeid(E)))
        {
            promise.SetEvent(e);

            _runningNodeCoroutine.resume();

            Traverse();
        }
    }
}
