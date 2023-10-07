#include "zerosugar/core/execution/channel/async_enumerable.h"
#include "zerosugar/core/execution/executor/executor_coroutine_traits.h"
#include "zerosugar/core/execution/executor/static_thread_pool.h"
#include "zerosugar/core/execution/executor/impl/asio_executor.h"
#include "zerosugar/core/execution/future/future.hpp"

using zerosugar::execution::AsyncEnumerable;
using zerosugar::execution::StaticThreadPool;
using zerosugar::execution::IExecutor;
using zerosugar::execution::ExecutionContext;
using zerosugar::execution::Future;

class AsyncEnumerableTest : public ::testing::Test
{
public:
    AsyncEnumerableTest()
        : _asioExecutor(std::make_shared<zerosugar::execution::executor::AsioExecutor>(4))
        , executor(*_asioExecutor)
    {
    }

    ~AsyncEnumerableTest() = default;

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

TEST_F(AsyncEnumerableTest, YieldIntValue)
{
    constexpr int32_t expected0 = 1;
    constexpr int32_t expected1 = 2;
    constexpr int32_t expected2 = 3;
    constexpr int32_t expected3 = 4;

    struct TestContext
    {
        std::atomic<bool> waiter = false;
        std::vector<int32_t> result;
    };

    // arrange
    TestContext context;

    // act
    Post(executor, [](TestContext& context) -> Future<void>
        {
            auto asyncEnumerableFunction = []() -> AsyncEnumerable<int32_t>
                {
                    co_yield expected0;
                    co_yield expected1;

                    std::vector<int32_t> range{ expected2, expected3 };
                    co_yield range;
                };

            AsyncEnumerable<int32_t> asyncEnumerable = asyncEnumerableFunction();

            while (asyncEnumerable.HasNext())
            {
                context.result.push_back(co_await asyncEnumerable);
            }

            context.waiter.store(true);
            context.waiter.notify_one();

        }, std::ref(context));

    context.waiter.wait(false);

    // assert
    ASSERT_EQ(context.result.size(), 4);
    EXPECT_EQ(context.result[0], expected0);
    EXPECT_EQ(context.result[1], expected1);
    EXPECT_EQ(context.result[2], expected2);
    EXPECT_EQ(context.result[3], expected3);
}

TEST_F(AsyncEnumerableTest, YieldIntValue_ResumeExecutor)
{
    constexpr int32_t expected0 = 1;
    constexpr int32_t expected1 = 2;

    struct TestContext
    {
        IExecutor& innerExecutor;
        std::atomic<bool> waiter = false;
        std::vector<int32_t> result;

        IExecutor* innerExecutor1 = nullptr;
        IExecutor* innerExecutor2 = nullptr;
        IExecutor* outerExecutor1 = nullptr;
        std::vector<IExecutor*> outerExecutor2;
    };

    // arrange
    IExecutor& threadPool = static_cast<IExecutor&>(StaticThreadPool::GetInstance());
    TestContext context{
        .innerExecutor = threadPool,
    };

    // act
    Post(executor, [](TestContext& context) -> Future<void>
        {
            auto asyncEnumerableFunction = [](TestContext& context) -> AsyncEnumerable<int32_t>
                {
                    co_await context.innerExecutor;

                    context.innerExecutor1 = ExecutionContext::GetExecutor();
                    co_yield expected0;

                    co_await zerosugar::execution::Delay(std::chrono::milliseconds(200));

                    context.innerExecutor2 = ExecutionContext::GetExecutor();
                    co_yield expected1;
                };

            AsyncEnumerable<int32_t> asyncEnumerable = asyncEnumerableFunction(context);

            context.outerExecutor1 = ExecutionContext::GetExecutor();

            while (asyncEnumerable.HasNext())
            {
                context.result.push_back(co_await asyncEnumerable);

                context.outerExecutor2.push_back(ExecutionContext::GetExecutor());
            }

            context.waiter.store(true);
            context.waiter.notify_one();

        }, std::ref(context));

    context.waiter.wait(false);

    // assert
    ASSERT_EQ(context.result.size(), 2);
    EXPECT_EQ(context.result[0], expected0);
    EXPECT_EQ(context.result[1], expected1);

    EXPECT_EQ(context.innerExecutor1, &context.innerExecutor);
    EXPECT_EQ(context.innerExecutor2, &context.innerExecutor);

    EXPECT_EQ(context.outerExecutor1, &executor);

    for (const IExecutor* outerExecutor : context.outerExecutor2)
    {
        EXPECT_EQ(outerExecutor, &executor);
    }
}

TEST_F(AsyncEnumerableTest, YieldException)
{
    struct TestContext
    {
        std::atomic<bool> waiter = false;
        std::exception_ptr exception = nullptr;

        IExecutor& innerExecutor;
    };

    // arrange
    IExecutor& threadPool = static_cast<IExecutor&>(StaticThreadPool::GetInstance());
    TestContext context{
        .innerExecutor = threadPool,
    };

    // act
    Post(executor, [](TestContext& context) -> Future<void>
        {
            auto asyncEnumerableFunction = [](TestContext& context) -> AsyncEnumerable<int32_t>
                {
                    co_await context.innerExecutor;

                    throw std::runtime_error("test");
                };

            AsyncEnumerable<int32_t> asyncEnumerable = asyncEnumerableFunction(context);

            try
            {
                while (asyncEnumerable.HasNext())
                {
                    (void)co_await asyncEnumerable;
                }
            }
            catch (...)
            {
                context.exception = std::current_exception();
            }

            context.waiter.store(true);
            context.waiter.notify_one();

        }, std::ref(context));

    context.waiter.wait(false);

    // assert
    EXPECT_PRED1([](const std::exception_ptr& exception)
        {
            return exception != nullptr;
        }, context.exception);
}
