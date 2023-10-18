#include "zerosugar/shared/execution/channel/channel.h"

using zerosugar::Channel;

TEST(Channel, Construct)
{
    // arrange
    const Channel<int32_t> channel;

    // act
    bool open = channel.IsOpen();
    bool closed = channel.IsClosed();

    // assert
    EXPECT_TRUE(open);
    EXPECT_FALSE(closed);
}

TEST(Channel, WriteReadOnClosedStatus)
{
    constexpr int32_t expected = 1234;

    // arrange
    Channel<std::optional<int32_t>> channel;

    // act
    const bool closed = channel.Close();

    channel.Send(expected);

    std::optional<int32_t> result1 = std::nullopt;
    const bool read1 = channel.Receive(result1);

    // assert
    EXPECT_TRUE(closed);

    EXPECT_TRUE(read1);
    EXPECT_TRUE(result1.has_value());
}


TEST(Channel, SendReceive)
{
    constexpr int32_t expected = 1234;

    // arrange
    Channel<std::optional<int32_t>> channel;

    // act
    channel.Send(expected);

    std::optional<int32_t> result1 = std::nullopt;
    const bool receive1 = channel.Receive(result1);

    std::optional<int32_t> result2 = std::nullopt;
    const bool receive2 = channel.Receive(result2);

    // assert
    EXPECT_TRUE(receive1);
    EXPECT_EQ(result1.value_or(~expected), expected);

    EXPECT_FALSE(receive2);
    EXPECT_FALSE(result2.has_value());
}

TEST(Channel, SendRangeReceive)
{
    std::vector<std::optional<int32_t>> expected{ 15235, 476321123, 123123, 64643, };

    // arrange
    Channel<std::optional<int32_t>> channel;
    std::vector<std::optional<int32_t>> result1;

    // act
    channel.Send(expected);

    std::optional<int32_t> item;
    while (channel.Receive(item))
    {
        result1.push_back(item);
    }

    // assert
    ASSERT_EQ(result1.size(), expected.size());
    for (size_t i = 0; i < result1.size(); ++i)
    {
        EXPECT_EQ(result1[i], expected[i]);
    }
}
