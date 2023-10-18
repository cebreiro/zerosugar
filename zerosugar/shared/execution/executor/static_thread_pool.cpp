#include "static_thread_pool.h"

#include <thread>
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"

namespace zerosugar::execution
{
    StaticThreadPool::StaticThreadPool()
        : _executor([]()
            {
                auto e = std::make_shared<executor::AsioExecutor>(
                    static_cast<int64_t>(std::thread::hardware_concurrency()));
                e->Run();

                return e;
            }())
    {
    }

    void StaticThreadPool::Delay(const std::function<void()>& function, std::chrono::milliseconds milliseconds)
    {
        auto executor = static_cast<executor::AsioExecutor*>(_executor.get());

        executor->Delay(function, milliseconds);
    }

    void StaticThreadPool::Delay(std::move_only_function<void()> function, std::chrono::milliseconds milliseconds)
    {
        auto executor = static_cast<executor::AsioExecutor*>(_executor.get());

        executor->Delay(std::move(function), milliseconds);
    }

    StaticThreadPool::operator IExecutor& ()
    {
        return *_executor;
    }

    StaticThreadPool::operator const IExecutor& () const
    {
        return *_executor;
    }

    auto StaticThreadPool::GetInstance() -> StaticThreadPool&
    {
        static StaticThreadPool instance;
        return instance;
    }
}
