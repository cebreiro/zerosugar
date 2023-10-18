#include "execution_context.h"

#include <cassert>

namespace zerosugar::execution
{
    thread_local std::vector<std::pair<IExecutor*, std::thread::id>> ExecutionContext::_executors;
    thread_local std::vector<std::pair<ICancelable*, std::thread::id>> ExecutionContext::_cancelables;

    void ExecutionContext::PushExecutor(PtrNotNull<IExecutor> executor, std::thread::id id)
    {
        _executors.emplace_back(executor, id);
    }

    void ExecutionContext::PopExecutor(std::thread::id id)
    {
        assert(!_executors.empty());
        assert(_executors.back().second == id);

        _executors.pop_back();
    }

    void ExecutionContext::PushCancelable(PtrNotNull<ICancelable> cancelable, std::thread::id id)
    {
        _cancelables.emplace_back(cancelable, id);
    }

    void ExecutionContext::PopCancelable(std::thread::id id)
    {
        assert(!_cancelables.empty());
        assert(_cancelables.back().second == id);

        _cancelables.pop_back();
    }

    auto ExecutionContext::GetExecutor() -> IExecutor*
    {
        return _executors.back().first;
    }

    auto ExecutionContext::GetCancelable() -> ICancelable&
    {
        assert(!_cancelables.empty());
        return *_cancelables.back().first;
    }

    ExecutionContext::ExecutorGuard::ExecutorGuard(PtrNotNull<IExecutor> executor)
    {
        PushExecutor(executor);
    }

    ExecutionContext::ExecutorGuard::~ExecutorGuard()
    {
        PopExecutor();
    }

    ExecutionContext::CancelableGuard::CancelableGuard(PtrNotNull<ICancelable> cancelable)
    {
        PushCancelable(cancelable);
    }

    ExecutionContext::CancelableGuard::~CancelableGuard()
    {
        PopCancelable();
    }
}
