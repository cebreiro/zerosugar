#pragma once
#include <vector>
#include <thread>
#include "zerosugar/shared/type/not_null_pointer.h"

namespace zerosugar::execution
{
    class IExecutor;
    class ICancelable;

    class ExecutionContext
    {
    public:
        ExecutionContext() = delete;

    private:
        static void PushExecutor(PtrNotNull<IExecutor> executor, std::thread::id id = std::this_thread::get_id());
        static void PopExecutor(std::thread::id id = std::this_thread::get_id());

        static void PushCancelable(PtrNotNull<ICancelable> cancelable, std::thread::id id = std::this_thread::get_id());
        static void PopCancelable(std::thread::id id = std::this_thread::get_id());

    public:
        static auto GetExecutor() -> IExecutor*;
        static auto GetCancelable() -> ICancelable&;

    public:
        struct ExecutorGuard
        {
            ExecutorGuard() = delete;

            explicit ExecutorGuard(PtrNotNull<IExecutor> executor);
            ~ExecutorGuard();
        };

        struct CancelableGuard
        {
            CancelableGuard() = delete;

            explicit CancelableGuard(PtrNotNull<ICancelable> cancelable);
            ~CancelableGuard();
        };

    private:
        static thread_local std::vector<std::pair<IExecutor*, std::thread::id>> _executors;
        static thread_local std::vector<std::pair<ICancelable*, std::thread::id>> _cancelables;
    };
}
