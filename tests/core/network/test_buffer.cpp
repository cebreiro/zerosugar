#include "zerosugar/core/network/buffer/buffer.h"

using zerosugar::network::Buffer;
using zerosugar::network::buffer::Fragment;

TEST(Buffer, DefaultConstruct)
{
    // arrange
    Buffer buffer;

    // act

    // assert
    EXPECT_EQ(buffer.GetSize(), 0);
    EXPECT_EQ(buffer.begin(), buffer.end());
}

TEST(Buffer, AddEmptyFragment)
{
    // arrange
    Buffer buffer;

    // act
    buffer.Add(Fragment{});

    // assert
    EXPECT_EQ(buffer.GetSize(), 0);
    EXPECT_EQ(std::distance(buffer.begin(), buffer.end()), 0);
}

TEST(Buffer, AddFragmentMultiple)
{
    // arrange
    constexpr int64_t size = 16;
    constexpr int64_t count = 3;

    Buffer buffer;

    // act
    for (int64_t i = 0; i < count; ++i)
    {
        buffer.Add(Fragment{ std::make_shared<char[]>(size), 0, size });
    }

    // assert
    EXPECT_EQ(buffer.GetSize(), size * count);
    EXPECT_EQ(std::distance(buffer.begin(), buffer.end()), buffer.GetSize());
}

TEST(Buffer, Merge)
{
    // arrange
    constexpr int64_t size = 16;
    constexpr int64_t count = 3;

    Buffer buffer1;
    for (int64_t i = 0; i < count; ++i)
    {
        buffer1.Add(Fragment{ std::make_shared<char[]>(size), 0, size });
    }

    Buffer buffer2;
    buffer2.Add(Fragment{ std::make_shared<char[]>(size), 0, size });

    // act
    buffer2.MergeBack(std::move(buffer1));

    // assert
    EXPECT_EQ(buffer1.GetSize(), 0);
    EXPECT_EQ(std::distance(buffer1.begin(), buffer1.end()), buffer1.GetSize());

    EXPECT_EQ(buffer2.GetSize(), size * (count + 1));
    EXPECT_EQ(std::distance(buffer2.begin(), buffer2.end()), buffer2.GetSize());
}

TEST(Buffer, SliceFront_SliceFragmentHalf)
{
    // arrange
    constexpr int64_t size = 32;
    constexpr int64_t halfSize = size / 2;

    auto memory = std::make_shared<char[]>(size);
    for (int64_t i = 0; i < size; ++i)
    {
        memory[i] = static_cast<char>(i % halfSize);
    }

    Buffer buffer1;
    buffer1.Add(Fragment{ std::move(memory), 0, size });

    // act
    Buffer buffer2;
    const bool sliced = buffer1.SliceFront(buffer2, halfSize);

    // assert
    EXPECT_EQ(buffer1.GetSize(), halfSize);
    EXPECT_EQ(std::distance(buffer1.begin(), buffer1.end()), buffer1.GetSize());

    ASSERT_TRUE(sliced);
    EXPECT_EQ(buffer2.GetSize(), halfSize);
    EXPECT_EQ(std::distance(buffer2.begin(), buffer2.end()), buffer2.GetSize());

    for (int64_t i = 0; i < halfSize; ++i)
    {
        auto iter1 = buffer1.begin();
        auto iter2 = buffer2.begin();

        std::advance(iter1, i);
        std::advance(iter2, i);

        EXPECT_EQ(*iter1, static_cast<char>(i));
        EXPECT_EQ(*iter2, static_cast<char>(i));
    }
}

TEST(Buffer, IteratorRead)
{
    // arrange
    constexpr int64_t FragmentSize = 32;
    constexpr int64_t FragmentUsedSize = 16;
    constexpr int64_t count = 3;

    std::vector<std::shared_ptr<char[]>> memories;

    int64_t k = 0;
    for (int64_t i = 0; i < count; ++i)
    {
        auto memory = std::make_shared<char[]>(FragmentSize);
        for (int64_t j = 0; j < FragmentUsedSize; ++j)
        {
            memory[j] = static_cast<char>(k++);
        }

        memories.push_back(std::move(memory));
    }

    Buffer buffer;
    buffer.Add(Fragment{});

    for (std::shared_ptr<char[]>& memory : memories)
    {
        Fragment Fragment(std::move(memory), 0, FragmentUsedSize);

        buffer.Add(std::move(Fragment));
    }

    Buffer::iterator begin = buffer.begin();
    Buffer::iterator end = buffer.end();

    // act
    std::vector<char> result(begin, end);

    // assert
    EXPECT_EQ(std::ssize(result), FragmentUsedSize * 3);
    EXPECT_EQ(std::distance(begin, end), buffer.GetSize());

    for (int64_t i = 0; i < FragmentUsedSize * count; ++i)
    {
        EXPECT_EQ(result[i], static_cast<char>(i));
    }
}
