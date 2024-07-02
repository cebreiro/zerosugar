#include "behavior_tree.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_interface.h"
#include "zerosugar/shared/ai/behavior_tree/node/bt_node.h"
#include "zerosugar/shared/ai/behavior_tree/node/bt_node_result.h"

namespace zerosugar
{
    BehaviorTree::BehaviorTree(bt::BlackBoard& blackBoard)
        : _blackBoard(blackBoard)
    {
    }

    BehaviorTree::~BehaviorTree()
    {
        Finalize();
    }

    void BehaviorTree::Initialize(std::vector<UniquePtrNotNull<bt::INode>> nodes)
    {
        assert(!nodes.empty());

        _storage = std::move(nodes);
        _rootNode = _storage.front()->GetNodePtr();

        for (const UniquePtrNotNull<bt::INode>& node : _storage)
        {
            bt::NodePtr nodePtr = node->GetNodePtr();

            if (auto pptr = std::get_if<PtrNotNull<bt::Leaf>>(&nodePtr); pptr)
            {
                bt::Leaf& leaf = **pptr;

                leaf.SetBehaviorTree(*this);
            }
        }
    }

    bool BehaviorTree::IsAwaiting() const
    {
        return !_stack.empty();
    }

    bool BehaviorTree::StopRequested() const
    {
        return _stopRequested;
    }

    void BehaviorTree::RunOnce()
    {
        assert(!IsAwaiting());

        _stack.push_back(_rootNode);

        Traverse();
    }

    void BehaviorTree::RequestStop()
    {
        _stopRequested = true;
    }

    bool BehaviorTree::IsWaitFor(const std::type_info& typeInfo) const
    {
        if (_currentState != bt::node::State::Running)
        {
            return false;
        }

        assert(_runningNodeCoroutine);
        bt::node::Result::promise_type& promise = _runningNodeCoroutine.promise();

        return promise.IsWaitingFor(typeInfo);
    }

    void BehaviorTree::Finalize()
    {
        FinalizeTraverse();

        _stack.clear();

        if (_runningNodeCoroutine)
        {
            _runningNodeCoroutine = nullptr;
        }
    }

    void BehaviorTree::Notify(const std::any& any)
    {
        if (_currentState != bt::node::State::Running)
        {
            return;
        }

        assert(_runningNodeCoroutine);

        if (bt::node::Result::promise_type& promise = _runningNodeCoroutine.promise();
            promise.IsWaitingFor(any.type()))
        {
            promise.SetEvent(any);

            _runningNodeCoroutine.resume();

            Traverse();
        }
    }

    auto BehaviorTree::GetBlackBoard() -> bt::BlackBoard&
    {
        return _blackBoard;
    }

    auto BehaviorTree::GetBlackBoard() const -> const bt::BlackBoard&
    {
        return _blackBoard;
    }

    void BehaviorTree::Traverse()
    {
        assert(!_stack.empty());

        while (!_stack.empty())
        {
            bt::NodePtr current = _stack.back();

            if (auto pptr = std::get_if<PtrNotNull<bt::Branch>>(&current); pptr)
            {
                bt::Branch& branch = **pptr;

                if (_visited.contains(&branch))
                {
                    assert(_currentState != bt::node::State::Running);

                    if (branch.ShouldContinue(_currentState))
                    {
                        if (branch.HasNext())
                        {
                            _stack.emplace_back(branch.Next());
                        }
                        else
                        {
                            _stack.pop_back();
                        }
                    }
                    else
                    {
                        _stack.pop_back();
                    }
                }
                else
                {
                    assert(branch.HasNext());

                    (void)_visited.insert(&branch);

                    _stack.emplace_back(branch.Next());
                }
            }
            if (auto pptr = std::get_if<PtrNotNull<bt::Decorator>>(&current); pptr)
            {
                bt::Decorator& decorator = **pptr;

                if (_visited.contains(&decorator))
                {
                    assert(_currentState != bt::node::State::Running);

                    _currentState = decorator.Decorate(_currentState);

                    _stack.pop_back();
                }
                else
                {
                    assert(decorator.HasChild());

                    (void)_visited.insert(&decorator);

                    _stack.push_back(decorator.GetChild());
                }
            }
            if (auto pptr = std::get_if<PtrNotNull<bt::Leaf>>(&current); pptr)
            {
                bt::Leaf& leaf = **pptr;

                if (_visited.contains(&leaf))
                {
                    assert(_currentState == bt::node::State::Running);
                    assert(_runningNodeCoroutine && _runningNodeCoroutine == leaf.GetPromise());

                    _currentState = _runningNodeCoroutine.promise().GetState();

                    if (_currentState == bt::node::State::Running)
                    {
                        break;
                    }
                    else
                    {
                        _runningNodeCoroutine = nullptr;
                        _stack.pop_back();
                    }
                }
                else
                {
                    _currentState = leaf.Execute();
                    _visited.insert(&leaf);

                    if (_currentState == bt::node::State::Running)
                    {
                        _runningNodeCoroutine = leaf.GetPromise();

                        break;
                    }

                    _stack.pop_back();
                }
            }
        }

        if (_stack.empty())
        {
            FinalizeTraverse();
        }
    }

    void BehaviorTree::FinalizeTraverse()
    {
        assert(_runningNodeCoroutine == nullptr);

        for (bt::INode* node : _visited)
        {
            node->Reset();
        }

        _visited.clear();
    }
}
