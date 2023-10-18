#include "zerosugar/shared/execution/executor/executor.hpp"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"

#include <atomic>
#include <future>

class ExecutorOperationTest : public ::testing::Test
{
public:
    ExecutorOperationTest()
        : _asioExecutor(std::make_shared<zerosugar::execution::executor::AsioExecutor>(4))
        , executor(*_asioExecutor)
    {
    }

    ~ExecutorOperationTest() = default;

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
    zerosugar::execution::IExecutor& executor;
};

class CopyOnlyType
{
public:
    CopyOnlyType(CopyOnlyType&&) noexcept = delete;
    CopyOnlyType& operator=(CopyOnlyType&&) noexcept = delete;

    CopyOnlyType(const CopyOnlyType& rhs) = default;
    CopyOnlyType& operator=(const CopyOnlyType& rhs) = default;
    CopyOnlyType() = default;
    explicit CopyOnlyType(int32_t value) : _value(value) {}

    auto GetValue() const ->int32_t {
        return _value;
    }

private:
    int32_t _value = 0;
};

constexpr int32_t ATOMIC_WAIT = 0;

TEST_F(ExecutorOperationTest, PostOnlyMovableNoParam)
{
    constexpr int32_t EXPECTED = 10;

    // arrange
    std::packaged_task<int32_t()> moveOnlyOperation([]() { return EXPECTED; });
    std::future<int32_t> future = moveOnlyOperation.get_future();

    // act
    Post(executor, std::move(moveOnlyOperation));

    const int32_t result = future.get();

    // assert
    EXPECT_EQ(result, EXPECTED);
}

TEST_F(ExecutorOperationTest, PostOnlyCopyableNoParam)
{
    constexpr int32_t EXPECTED = 10;

    // arrange
    CopyOnlyType copyOnlyType(EXPECTED);
    std::atomic<int32_t> result = ATOMIC_WAIT;

    // act
    Post(executor, [copyOnlyType, &result]()
        {
            result.store(copyOnlyType.GetValue());
            result.notify_one();
        });

    result.wait(ATOMIC_WAIT);

    // assert
    EXPECT_EQ(result.load(), EXPECTED);
}

TEST_F(ExecutorOperationTest, PostOnlyMovableMultipleParam)
{
    constexpr int32_t PARAM_1 = 3;
    constexpr int32_t PARAM_2 = 4;
    constexpr int32_t PARAM_3 = 5;
    constexpr int32_t EXPECTED = PARAM_1 * PARAM_2 * PARAM_3;

    // arrange
    std::atomic<int32_t> result = ATOMIC_WAIT;
    auto param1 = std::make_unique<int32_t>(PARAM_1);
    auto param2 = std::make_unique<int32_t>(PARAM_2);
    auto param3 = std::make_shared<int32_t>(PARAM_3);

    auto task = [&result, param1 = std::move(param1)](std::unique_ptr<int32_t> param2, std::shared_ptr<int32_t> param3)
        {
            result.store(*param1 * *param2 * *param3);
            result.notify_one();
        };

    // act
    Post(executor, std::move(task), std::move(param2), param3);

    result.wait(ATOMIC_WAIT);

    // assert
    EXPECT_EQ(result.load(), EXPECTED);
}

TEST_F(ExecutorOperationTest, PostOnlyCopyableMultipleParam)
{
    constexpr int32_t PARAM_1 = 3;
    constexpr int32_t PARAM_2 = 4;
    constexpr int32_t PARAM_3 = 5;
    constexpr int32_t EXPECTED = PARAM_1 * PARAM_2 * PARAM_3;

    // arrange
    std::atomic<int32_t> result = ATOMIC_WAIT;
    CopyOnlyType param1(PARAM_1);
    CopyOnlyType param2(PARAM_2);
    CopyOnlyType param3(PARAM_3);

    auto task = [&result, param1](CopyOnlyType param2, CopyOnlyType param3)
        {
            result.store(param1.GetValue() * param2.GetValue() * param3.GetValue());
            result.notify_one();
        };

    // act
    Post(executor, std::move(task), param2, param3);

    result.wait(ATOMIC_WAIT);

    // assert
    EXPECT_EQ(result.load(), EXPECTED);
}
