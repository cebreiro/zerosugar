#include "zerosugar/shared/execution/executor/strand.h"
#include "zerosugar/shared/execution/executor/static_thread_pool.h"
#include "zerosugar/shared/execution/executor/operation/dispatch.h"
#include "zerosugar/shared/execution/executor/operation/post.h"

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

TEST(Strand, Dispatch)
{
    std::atomic<bool> done = false;

    // arrange
    auto& executor = static_cast<IExecutor&>(StaticThreadPool::GetInstance());
    auto strand = std::make_shared<Strand>(executor.SharedFromThis());

    std::vector<int32_t> result;

    // act
    Post(*strand, [&strand, &result, &done]()
        {
            Post(*strand, [&result, &done]()
                {
                    result.push_back(2);

                    if (result.size() == 3)
                    {
                        done.store(true);
                        done.notify_one();
                    }
                });
            Dispatch(*strand, [&result, &done]()
                {
                    result.push_back(1);

                    if (result.size() == 3)
                    {
                        done.store(true);
                        done.notify_one();
                    }
                });
            Dispatch(*strand, [&result, &done]()
                {
                    result.push_back(0);

                    if (result.size() == 3)
                    {
                        done.store(true);
                        done.notify_one();
                    }
                });
        });

    done.wait(false);

    // assert
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 0);
    EXPECT_EQ(result[1], 1);
    EXPECT_EQ(result[2], 2);
}
