#include "zerosugar/core/execution/executor/strand.h"
#include "zerosugar/core/execution/executor/static_thread_pool.h"
#include "zerosugar/core/execution/executor/operation/post.h"

using zerosugar::execution::IExecutor;
using zerosugar::execution::Strand;
using zerosugar::execution::StaticThreadPool;


TEST(ThreadPool_Strand, SimpleArithmeticAdd)
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
        Post(*strand, std::function([&result, &counter]()
            {
                ++result;
                counter.fetch_add(1);
            }));
    }

    while (counter.load() != expected)
    {
    }

    // assert
    EXPECT_EQ(result, expected);
}
