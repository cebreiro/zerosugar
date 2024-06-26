#include "zerosugar/shared/execution/executor/executor_coroutine_traits.h"
#include "zerosugar/shared/execution/executor/strand.h"
#include "zerosugar/shared/execution/executor/static_thread_pool.h"
#include "zerosugar/shared/execution/executor/operation/dispatch.h"
#include "zerosugar/shared/execution/executor/operation/post.h"
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/execution/future/future_coroutine_traits.h"

using zerosugar::execution::IExecutor;
using zerosugar::Strand;
using zerosugar::StaticThreadPool;


TEST(Strand, SimpleArithmeticAdd)
{
    // arrange
    auto& executor = static_cast<IExecutor&>(StaticThreadPool::GetInstance());
    auto strand = std::make_shared<Strand>(executor.SharedFromThis());

    int32_t expected = 100000;
    int32_t result = 0;
    std::atomic<int32_t> counter = 0;

    // act
    for (int32_t i = 0; i < expected; ++i)
    {
        Post(*strand, [&result, &counter]()
            {
                ++result;
                counter.fetch_add(1);
            });
    }

    while (counter.load() != expected)
    {
    }

    // assert
    EXPECT_EQ(result, expected);
}

TEST(Strand, Dispatch)
{
    std::atomic<bool> done = false;

    // arrange
    auto& executor = static_cast<IExecutor&>(StaticThreadPool::GetInstance());
    auto strand = std::make_shared<Strand>(executor.SharedFromThis());

    std::vector<int32_t> result;

    const auto notify = [&]()
        {
            if (result.size() == 3)
            {
                done.store(true);
                done.notify_one();
            }
        };

    // act
    Post(*strand, [&strand, &result, &notify]()
        {
            Post(*strand, [&result, &notify]()
                {
                    result.push_back(2);

                    notify();
                });
            Dispatch(*strand, [&result, &notify]()
                {
                    result.push_back(0);

                    notify();
                });
            Dispatch(*strand, [&result, &notify]()
                {
                    result.push_back(1);

                    notify();
                });
        });

    done.wait(false);

    // assert
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 0); // 2
    EXPECT_EQ(result[1], 1); // 0
    EXPECT_EQ(result[2], 2); // 1
}

TEST(Strand, AwaitStrandExecutor)
{
    std::atomic<bool> done = false;

    // arrange
    auto& executor = static_cast<IExecutor&>(StaticThreadPool::GetInstance());
    auto strand = std::make_shared<Strand>(executor.SharedFromThis());

    using namespace zerosugar;

    // act
    Post(*strand, [](SharedPtrNotNull<Strand> strand, std::atomic<bool>& done) -> Future<void>
        {
            co_await *strand;

            co_await *strand;

            done.store(true);
            done.notify_one();

            co_return;
        }, strand, std::ref(done));

    done.wait(false);

    // assert
}

TEST(Strand, AwaitStrandMultpleThreads)
{
    std::atomic<int64_t> count = 0;
    std::atomic<bool> done = false;

    // arrange
    auto& executor = static_cast<IExecutor&>(StaticThreadPool::GetInstance());
    auto strand = std::make_shared<Strand>(executor.SharedFromThis());

    using namespace zerosugar;

    const auto notify = [&]()
        {
            if (count.fetch_add(1) == 999)
            {
                done.store(true);
                done.notify_one();
            }
        };

    // act
    for (size_t i = 0; i < 1000; ++i)
    {
        Post(executor, [](SharedPtrNotNull<Strand> strand, auto notify) -> Future<void>
            {
                co_await *strand;

                notify();

                co_return;
            }, strand, notify);
    }

    done.wait(false);

    // assert
}

TEST(Strand, HandlersExecuteSequentially)
{
    std::atomic<bool> done = false;

    // arrange
    auto& executor = static_cast<IExecutor&>(StaticThreadPool::GetInstance());
    auto strand = std::make_shared<Strand>(executor.SharedFromThis());

    std::atomic<int64_t> counter = 0;
    std::vector<int64_t> results;

    const auto handler = [&counter, &results, &done](int64_t id)
        {
            const int64_t value = counter.load();
            results.push_back(id);

            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            const int64_t next = value + 1;
            counter.store(next);

            if (next == 10)
            {
                done.store(true);
                done.notify_all();
            }
        };

    // act
    for (int64_t i = 0; i < 10; ++i)
    {
        Post(*strand, [i, &handler]()
            {
                handler(i);
            });
    }

    done.wait(false);

    // assert
    ASSERT_EQ(counter.load(), 10);

    for (int64_t i = 0; i < 10; ++i)
    {
        ASSERT_EQ(results[i], i);
    }
}
