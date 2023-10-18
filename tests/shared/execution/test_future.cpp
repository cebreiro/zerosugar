#include "zerosugar/shared/execution/executor/strand.h"
#include "zerosugar/shared/execution/executor/executor_coroutine_traits.h"
#include "zerosugar/shared/execution/executor/static_thread_pool.h"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/execution/future/future.hpp"

using zerosugar::execution::IExecutor;
using zerosugar::execution::Strand;
using zerosugar::execution::StaticThreadPool;
using zerosugar::execution::Future;
using zerosugar::execution::ExecutionContext;

class FutureTest : public ::testing::Test
{
public:
    FutureTest()
        : _asioExecutor(std::make_shared<zerosugar::execution::executor::AsioExecutor>(4))
        , executor(*_asioExecutor)
    {
    }

    ~FutureTest() = default;

    void SetUp() override
    {
        _asioExecutor->Run();
    }

    void TearDown() override
    {
        _asioExecutor->Stop();
        _asioExecutor->Join(nullptr);
    }

private:
    std::shared_ptr<zerosugar::execution::executor::AsioExecutor> _asioExecutor;

protected:
    IExecutor& executor;
};

TEST_F(FutureTest, CreateFromLambda)
{
    // arrange
    constexpr int32_t expected = 123123;

    // act
    Future<int32_t> future = StartAsync(executor, []() -> int32_t
        {
            return expected;
        });

    // assert
    int32_t v = future.Get();
    EXPECT_EQ(v, expected);
}

TEST_F(FutureTest, CreateFromCapturingLambda)
{
    // arrange
    constexpr int32_t expected = 123123;

    // act
    Future<int32_t> future = StartAsync(executor, [result = expected]() -> int32_t
        {
            return result;
        });

    // assert
    EXPECT_EQ(future.Get(), expected);
    EXPECT_TRUE(future.IsComplete());
}

namespace test::detail
{
    constexpr int32_t expected = 123123;
    int32_t Function() {
        return expected;
    }
}

TEST_F(FutureTest, CreateFromFunction)
{
    // arrange

    // act
    Future<int32_t> future = StartAsync(executor, &test::detail::Function);

    // assert
    EXPECT_EQ(future.Get(), test::detail::expected);
    EXPECT_TRUE(future.IsComplete());
}

TEST_F(FutureTest, CreateFromMoveOnlyFunction)
{
    // arrange
    constexpr int32_t expected = 123123;
    auto item = std::make_unique<int32_t>(expected);

    // act
    Future<std::unique_ptr<int32_t>> future = StartAsync(executor, [item = std::move(item)]() mutable
        {
            return std::move(item);
        });

    // assert
    EXPECT_EQ(*future.Get(), expected);
    EXPECT_TRUE(future.IsComplete());
}

TEST_F(FutureTest, Cancel_Success)
{
    // arrange
    bool canceled = false;
    bool cancelRequested = false;
    std::atomic<bool> waiter = false;

    // act
    Future<void> future = StartAsync(executor, []()
        {
            // for delay
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        })
    .Then(executor, [&cancelRequested, &waiter]()
        {
            cancelRequested = ExecutionContext::GetCancelable().IsCanceled();

            waiter.store(true);
            waiter.notify_one();
        });

    canceled = future.Cancel();
    waiter.wait(false);

    // assert
    EXPECT_TRUE(canceled);
    EXPECT_TRUE(cancelRequested);
    EXPECT_ANY_THROW(future.Get());
    EXPECT_FALSE(future.IsPending());
    EXPECT_FALSE(future.IsComplete());
    EXPECT_TRUE(future.IsCanceled());
}

TEST_F(FutureTest, Cancel_Failure)
{
    // arrange
    bool canceled = false;
    bool cancelRequested = false;
    std::atomic<bool> waiter = false;

    // act
    Future<void> future = StartAsync(executor, [&cancelRequested, &waiter]()
        {
            cancelRequested = ExecutionContext::GetCancelable().IsCanceled();

            waiter.store(true);
            waiter.notify_one();
        });

    // for delay, already executed
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    canceled = future.Cancel();
    waiter.wait(false);

    // assert
    EXPECT_FALSE(canceled);
    EXPECT_FALSE(cancelRequested);
    EXPECT_NO_THROW(future.Get());
    EXPECT_FALSE(future.IsPending());
    EXPECT_TRUE(future.IsComplete());
    EXPECT_FALSE(future.IsCanceled());
}

TEST_F(FutureTest, Cancel_OperationAbortException)
{
    // arrange
    bool canceled = false;
    bool cancelRequested = false;
    bool executed = false;
    std::atomic<bool> waiter = false;

    // act
    Future<void> future = StartAsync(executor, []()
        {
            // for delay
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        })
    .Then(executor, [&cancelRequested, &waiter]()
        {
            cancelRequested = ExecutionContext::GetCancelable().IsCanceled();

            waiter.store(true);
            waiter.notify_one();
        });

    Future<void> next = future.Then(executor, [&executed]()
        {
            executed = true;
        });

    canceled = future.Cancel();

    next.Wait();
    waiter.wait(false);

    // assert
    EXPECT_TRUE(canceled);
    EXPECT_TRUE(cancelRequested);
    EXPECT_FALSE(executed);

    EXPECT_FALSE(future.IsPending());
    EXPECT_FALSE(future.IsComplete());
    EXPECT_TRUE(future.IsCanceled());

    EXPECT_ANY_THROW(next.Get());
    EXPECT_FALSE(next.IsPending());
    EXPECT_TRUE(next.IsComplete());
    EXPECT_FALSE(next.IsCanceled());
}

TEST_F(FutureTest, Then_NoParam)
{
    // arrange
    constexpr int32_t discard = 123123;
    constexpr int32_t expected = 456456;

    // act
    Future<int32_t> future = StartAsync(executor, []() { return discard; })
        .Then(executor, []() { return expected; });

    future.Wait();

    // assert
    EXPECT_EQ(future.Get(), expected);
}

TEST_F(FutureTest, Then_OneParam)
{
    // arrange
    constexpr int32_t expected1 = 123123;
    constexpr int32_t expected2 = 456456;

    int32_t result1 = 0;

    // act
    Future<int32_t> future = StartAsync(executor, []()
        {
            return expected1;
        })
    .Then(executor, [&result1](int32_t value)
        {
            result1 = value;
            return expected2;
        });
    future.Wait();

    // assert
    EXPECT_EQ(result1, expected1);
    EXPECT_EQ(future.Get(), expected2);
}

TEST_F(FutureTest, ContinuationWith_ReturnVoid)
{
    // arrange
    int32_t result = 0;
    constexpr int32_t expected = 123123;

    // act
    Future<void> future = StartAsync(executor, []() { return expected; })
        .ContinuationWith(executor, [&result](Future<int32_t>& f)
            {
                result = f.Get();
            });

    future.Wait();

    // assert
    EXPECT_EQ(result, expected);
}

TEST_F(FutureTest, ContinuationWith_ReturnValue)
{
    // arrange
    constexpr int32_t expected = 123123;

    // act
    Future<int32_t> future = StartAsync(executor, []() { return expected; })
        .ContinuationWith(executor, [](Future<int32_t>& f)
            {
                return f.Get();
            });

    // assert
    EXPECT_EQ(future.Get(), expected);
}

TEST_F(FutureTest, Exception_StoreANDGetException)
{
    // arrange

    // act
    Future<void> future = StartAsync(executor, []()
        {
            throw std::runtime_error("test");
        });

    // assert
    EXPECT_ANY_THROW(future.Get());
    EXPECT_TRUE(future.IsComplete());
}

TEST_F(FutureTest, Exception_DoubleGetException)
{
    // arrange

    // act
    Future<int32_t> future1 = StartAsync(executor, [] { return 0; });
    Future<void> future2 = future1.Then(executor, []()
        {
            throw std::runtime_error("test");
        });

    // even if 'Then' callable do not use param, it consumes future1
    future2.Wait();

    // assert
    EXPECT_ANY_THROW(future1.Get());
    EXPECT_TRUE(future1.IsComplete());

    EXPECT_ANY_THROW(future2.Get());
    EXPECT_TRUE(future1.IsComplete());
}

TEST_F(FutureTest, Exception_PropagateException)
{
    // arrange
    bool executed = false;

    // act
    Future<void> future1 = StartAsync(executor, []()
        {
            throw std::runtime_error("test");
        });
    Future<void> future2 = future1.Then(executor, [&executed]
        {
            // not executed
            executed = true;
        });

    future2.Wait();

    // assert
    EXPECT_FALSE(executed);
    EXPECT_TRUE(future1.IsComplete());
    EXPECT_ANY_THROW(future2.Get());
}

TEST_F(FutureTest, Delay)
{
    // arrange
    using clock_type = std::chrono::high_resolution_clock;
    using time_point_type = clock_type::time_point;

    const time_point_type now = clock_type::now();
    time_point_type end = {};
    const auto delay = std::chrono::milliseconds(50);

    // act
    zerosugar::execution::Delay(delay).Wait();
    end = clock_type::now();

    // assert
    EXPECT_GE(end - now, delay);
}

TEST_F(FutureTest, WaitAll)
{
    constexpr size_t expected1 = 123123;
    constexpr size_t expected2 = 456456;
    constexpr size_t expected3 = 789789;

    // arrange
    auto executor1 = std::make_shared<Strand>(executor.SharedFromThis());
    auto executor2 = std::make_shared<Strand>(executor.SharedFromThis());
    auto executor3 = std::make_shared<Strand>(executor.SharedFromThis());

    // act
    Future<size_t> f1 = StartAsync(*executor1, []()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return expected1;

        });

    Future<size_t> f2 = StartAsync(*executor2, []()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            return expected2;

        });

    Future<size_t> f3 = StartAsync(*executor3,[]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
            return expected3;

        });

    Future<void> waitAll = WaitAll(executor, f1, f2, f3);
    waitAll.Get();

    // assert
    EXPECT_TRUE(f1.IsComplete());
    EXPECT_TRUE(f2.IsComplete());
    EXPECT_TRUE(f3.IsComplete());
    EXPECT_EQ(f1.Get(), expected1);
    EXPECT_EQ(f2.Get(), expected2);
    EXPECT_EQ(f3.Get(), expected3);
}

TEST_F(FutureTest, WaitAny)
{
    // arrange
    auto executor1 = std::make_shared<Strand>(executor.SharedFromThis());
    auto executor2 = std::make_shared<Strand>(executor.SharedFromThis());
    auto executor3 = std::make_shared<Strand>(executor.SharedFromThis());

    std::atomic<size_t> counter = 0;
    size_t count1 = 0;
    size_t count2 = 0;
    size_t count3 = 0;

    // act
    Future<void> f1 = StartAsync(*executor1, [&]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            count1 = counter.fetch_add(1) + 1;
        });

    Future<void> f2 = StartAsync(*executor2, [&]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            count2 = counter.fetch_add(1) + 1;
        });

    Future<void> f3 = StartAsync(*executor3, [&]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            count3 = counter.fetch_add(1) + 1;
        });

    Future<void> waitAny = WaitAny(executor, f1, f2, f3);
    waitAny.Get();

    const size_t count4 = counter.load();
    const bool operation1Complete = f1.IsComplete();
    const bool operation2Pending = f2.IsPending();
    const bool operation3Pending = f3.IsPending();

    f2.Wait();
    f3.Wait();

    // assert
    EXPECT_TRUE(operation1Complete);
    EXPECT_TRUE(operation2Pending);
    EXPECT_TRUE(operation3Pending);
    EXPECT_EQ(count1, count4);
    EXPECT_EQ(count3, counter.load());
}


TEST_F(FutureTest, Coroutine_ResumeOnOriginExecutor)
{
    // arrange
    struct TestContext
    {
        IExecutor& executor;
        std::shared_ptr<Strand> strand1;
        std::shared_ptr<Strand> strand2;
        std::shared_ptr<Strand> strand3;

        IExecutor* innerExecutor1 = nullptr;
        IExecutor* innerExecutor2 = nullptr;
        IExecutor* innerExecutor3 = nullptr;

        IExecutor* outerExecutor1 = nullptr;
        IExecutor* outerExecutor2 = nullptr;
        IExecutor* outerExecutor3 = nullptr;

        IExecutor* outerChangedExecutor1 = nullptr;
        IExecutor* outerChangedExecutor2 = nullptr;
        IExecutor* outerChangedExecutor3 = nullptr;
    };

    TestContext context{
        .executor = executor,
        .strand1 = std::make_shared<Strand>(executor.SharedFromThis()),
        .strand2 = std::make_shared<Strand>(executor.SharedFromThis()),
        .strand3 = std::make_shared<Strand>(executor.SharedFromThis()),
    };

    auto coroutine = [](TestContext& context) -> Future<void>
        {
            context.outerExecutor1 = ExecutionContext::GetExecutor();

            co_await StartAsync(context.executor, [&context]()
                {
                    context.innerExecutor1 = ExecutionContext::GetExecutor();
                });

            context.outerExecutor2 = ExecutionContext::GetExecutor();

            co_await StartAsync(*context.strand2, [&context]()
                {
                    context.innerExecutor2 = ExecutionContext::GetExecutor();
                });

            context.outerExecutor3 = ExecutionContext::GetExecutor();

            co_await *context.strand3;

            context.outerChangedExecutor1 = ExecutionContext::GetExecutor();

            co_await zerosugar::execution::Delay(std::chrono::milliseconds(1));

            context.outerChangedExecutor2 = ExecutionContext::GetExecutor();

            co_await StartAsync(*context.strand2, [&context]()
                {
                    context.innerExecutor3 = ExecutionContext::GetExecutor();
                });

            context.outerChangedExecutor3 = ExecutionContext::GetExecutor();

            co_return;
        };

    // act
    std::async(std::launch::async, [coroutine, &context]()
        {
            Future<Future<void>> fut = StartAsync(*context.strand1, [coroutine, &context]()
                {
                    return coroutine(context);
                });
            fut.Get().Wait();
        }).wait();

    // assert
    EXPECT_EQ(context.innerExecutor1, &context.executor);
    EXPECT_EQ(context.innerExecutor2, context.strand2.get());

    EXPECT_EQ(context.outerExecutor1, context.strand1.get());
    EXPECT_EQ(context.outerExecutor2, context.strand1.get());
    EXPECT_EQ(context.outerExecutor3, context.strand1.get());

    EXPECT_EQ(context.outerChangedExecutor1, context.strand3.get());
    EXPECT_EQ(context.outerChangedExecutor2, context.strand3.get());
    EXPECT_EQ(context.outerChangedExecutor3, context.strand3.get());
}
