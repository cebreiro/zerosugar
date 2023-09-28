#include "delay.h"

#include "zerosugar/core/execution/executor/static_thread_pool.h"
#include "zerosugar/core/execution/future/shared_context.h"

namespace zerosugar::execution
{
    auto Delay(std::chrono::milliseconds milliseconds) -> Future<void>
    {
        auto& threadPool = StaticThreadPool::GetInstance();

        auto context = std::make_shared<future::SharedContext<void>>();
        context->SetExecutor(static_cast<IExecutor&>(threadPool).SharedFromThis());

        threadPool.Delay(std::function([context]() mutable
            {
                ExecutionContext::CancelableGuard guard(context.get());

                if (context->IsCanceled())
                {
                    return;
                }

                context->OnSuccess();

            }), milliseconds);

        return Future<void>(std::move(context));
    }
}
