#include <array>
#include <random>
#include <ranges>
#include <boost/container/small_vector.hpp>
#include "zerosugar/shared/io/stream_writer.h"
#include "zerosugar/shared/network/buffer/buffer.h"
#include "zerosugar/shared/network/buffer/buffer_reader.h"

using zerosugar::Buffer;
using zerosugar::buffer::Fragment;
using zerosugar::BufferReader;

std::mt19937 mt(std::random_device{}());

auto GenerateFragment(int64_t size) -> Fragment
{
    std::vector<char> item(size);

    std::generate_n(item.begin(), size, std::ref(mt));

    return Fragment::CreateFrom(item);
}

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
        buffer.Add(Fragment::Create(size));
    }

    // assert
    EXPECT_EQ(buffer.GetSize(), size * count);
    EXPECT_EQ(std::distance(buffer.begin(), buffer.end()), buffer.GetSize());
}

TEST(Buffer, MoveFragment)
{
    constexpr int64_t size = 16;

    // arrange
    Fragment fragment1 = GenerateFragment(size);
    Fragment fragment2;

    // assert precondition
    ASSERT_TRUE(fragment1.IsValid());
    ASSERT_EQ(fragment1.GetRefCount(), 1);
    ASSERT_NE(fragment1.GetData(), nullptr);
    ASSERT_EQ(fragment1.GetSize(), size);

    ASSERT_FALSE(fragment2.IsValid());
    ASSERT_EQ(fragment2.GetRefCount(), 0);
    ASSERT_EQ(fragment2.GetData(), nullptr);
    ASSERT_EQ(fragment2.GetSize(), 0);

    // act
    fragment2 = std::move(fragment1);

    // assert
    EXPECT_FALSE(fragment1.IsValid());
    EXPECT_EQ(fragment1.GetRefCount(), 0);
    EXPECT_EQ(fragment1.GetData(), nullptr);
    EXPECT_EQ(fragment1.GetSize(), 0);

    EXPECT_TRUE(fragment2.IsValid());
    EXPECT_EQ(fragment2.GetRefCount(), 1);
    EXPECT_NE(fragment2.GetData(), nullptr);
    EXPECT_EQ(fragment2.GetSize(), size);
}

TEST(Buffer, SwapFragment)
{
    constexpr int64_t size = 16;

    // arrange
    Fragment fragment1 = GenerateFragment(size);
    Fragment fragment2;

    // assert precondition
    ASSERT_TRUE(fragment1.IsValid());
    ASSERT_EQ(fragment1.GetRefCount(), 1);
    ASSERT_NE(fragment1.GetData(), nullptr);
    ASSERT_EQ(fragment1.GetSize(), size);

    ASSERT_FALSE(fragment2.IsValid());
    ASSERT_EQ(fragment2.GetRefCount(), 0);
    ASSERT_EQ(fragment2.GetData(), nullptr);
    ASSERT_EQ(fragment2.GetSize(), 0);

    // act
    std::swap(fragment1, fragment2);

    // assert
    EXPECT_FALSE(fragment1.IsValid());
    EXPECT_EQ(fragment1.GetRefCount(), 0);
    EXPECT_EQ(fragment1.GetData(), nullptr);
    EXPECT_EQ(fragment1.GetSize(), 0);

    EXPECT_TRUE(fragment2.IsValid());
    EXPECT_EQ(fragment2.GetRefCount(), 1);
    EXPECT_NE(fragment2.GetData(), nullptr);
    EXPECT_EQ(fragment2.GetSize(), size);
}

TEST(Buffer, SwallowCopyFragment)
{
    constexpr int64_t size = 16;

    // arrange
    Fragment fragment1 = GenerateFragment(size);

    // act
    Fragment fragment2 = fragment1.ShallowCopy();

    // assert
    EXPECT_TRUE(fragment1.IsValid());
    EXPECT_TRUE(fragment2.IsValid());
    EXPECT_EQ(fragment1.GetData(), fragment2.GetData());
    EXPECT_EQ(fragment1.GetSize(), fragment2.GetSize());
    EXPECT_EQ(fragment1.GetRefCount(), 2);
    EXPECT_EQ(fragment2.GetRefCount(), 2);
}

TEST(Buffer, SwallowCopyFragmentAndDestroy)
{
    constexpr int64_t size = 16;

    Fragment fragment1 = GenerateFragment(size);
    EXPECT_EQ(fragment1.GetRefCount(), 1);

    Fragment fragment2 = fragment1.ShallowCopy();
    EXPECT_EQ(fragment1.GetRefCount(), 2);

    fragment2.~Fragment();
    EXPECT_EQ(fragment1.GetRefCount(), 1);

    fragment1.~Fragment();
    EXPECT_EQ(fragment1.GetRefCount(), 0);
}

TEST(Buffer, SliceFragment)
{
    constexpr int64_t size = 16;

    // arrange
    Fragment fragment = Fragment::Create(size);

    Buffer buffer;

    // act
    for (int64_t i = 0; i < size; ++i)
    {
        fragment.GetData()[i] = static_cast<int8_t>(i);
    }

    for (int64_t i = 0; i < size - 1; ++i)
    {
        std::optional<Fragment> sliced = fragment.SliceFront(1);
        ASSERT_TRUE(sliced.has_value());

        buffer.Add(std::move(*sliced));
    }

    buffer.Add(std::move(fragment));

    // assert
    EXPECT_EQ(buffer.GetSize(), size);

    auto iter = buffer.begin();
    for (int64_t i = 0; i < size; ++i)
    {
        EXPECT_EQ(*iter, static_cast<int8_t>(i));

        ++iter;
    }

    for (const Fragment& f : buffer.GetFragmentContainer())
    {
        EXPECT_EQ(f.GetSize(), 1);
    }
}

TEST(Buffer, MergeFragment)
{
    constexpr int64_t size = 16;

    // arrange
    Fragment fragment1 = Fragment::Create(size);
    Fragment fragment2 = Fragment::Create(size);

    Buffer buffer;

    // act
    for (int64_t i = 0; i < size; ++i)
    {
        fragment1.GetData()[i] = static_cast<int8_t>(i);
        fragment2.GetData()[size - i - 1] = static_cast<int8_t>(i);
    }

    buffer.Add(std::move(fragment1));
    buffer.Add(std::move(fragment2));

    // assert
    EXPECT_EQ(buffer.GetSize(), size * 2);

    auto iter = buffer.begin();
    for (int64_t i = 0; i < size; ++i)
    {
        EXPECT_EQ(*iter, static_cast<int8_t>(i));

        ++iter;
    }

    for (int64_t i = 0; i < size; ++i)
    {
        EXPECT_EQ(*iter, static_cast<int8_t>(size - i - 1));

        ++iter;
    }
}

TEST(Buffer, WriteRawBytesOnFragment)
{
    constexpr int64_t size = 16;
    constexpr int64_t count = 5;

    // arrange
    Fragment writeData = GenerateFragment(16 * 5);

    Buffer buffer;
    for (int64_t i = 0; i < count; ++i)
    {
        buffer.Add(Fragment::Create(size));
    }

    const char* data = writeData.GetData();

    // act
    for (Fragment& fragment : buffer.GetFragmentContainer())
    {
        for (int64_t i = 0; i < fragment.GetSize(); ++i)
        {
            fragment.GetData()[i] = *data;

            ++data;
        }
    }

    // assert
    auto iter = buffer.begin();

    for (int64_t i = 0; i < size * count; ++i, ++iter)
    {
        EXPECT_EQ(*iter, writeData.GetData()[i]);
    }
}

TEST(Buffer, MergeBuffer)
{
    // arrange
    constexpr int64_t size = 16;
    constexpr int64_t count = 3;

    std::array<std::vector<char>, count> items{
        std::vector<char>(size),
        std::vector<char>(size),
        std::vector<char>(size)
    };

    for (int64_t i = 0; i < count; ++i)
    {
        std::generate_n(items[i].begin(), size, std::ref(mt));
    }

    Buffer buffer1;
    for (int64_t i = 0; i < count; ++i)
    {
        buffer1.Add(Fragment::CreateFrom(items[i]));
    }

    // act
    Buffer buffer2;
    buffer2.MergeBack(std::move(buffer1));

    // assert
    EXPECT_TRUE(buffer1.Empty());
    EXPECT_EQ(buffer1.GetSize(), 0);
    EXPECT_EQ(std::distance(buffer1.begin(), buffer1.end()), buffer1.GetSize());

    EXPECT_FALSE(buffer2.Empty());
    EXPECT_EQ(buffer2.GetSize(), size * count);
    EXPECT_EQ(std::distance(buffer2.begin(), buffer2.end()), buffer2.GetSize());

    auto range = items | std::views::join;
    EXPECT_EQ(buffer2.GetSize(), std::ranges::distance(range));

    auto iterItem = range.begin();

    for (auto iter = buffer2.begin(); iter != buffer2.end(); ++iter, ++iterItem)
    {
        EXPECT_EQ(*iter, *iterItem);
    }
}

TEST(Buffer, BufferWrite)
{
    constexpr int8_t expected1 = 0x41;
    constexpr int16_t expected2 = 0x6433;
    constexpr int32_t expected3 = 0x63415431;
    constexpr int64_t expected4 = 0x1234567887654321;

#pragma pack(push,1)
    struct Data
    {
        int8_t v1 = expected1;
        int16_t v2 = expected2;
        int32_t v3 = expected3;
        int64_t v4 = expected4;
    };
#pragma pack(pop)

    // arrange
    Data expected;

    boost::container::small_vector<char, 256> buffer;
    zerosugar::StreamWriter bufferWriter(buffer);

    // act
    bufferWriter.Write(expected1);
    bufferWriter.Write(expected2);
    bufferWriter.Write(expected3);
    bufferWriter.Write(expected4);

    // assert
    const Data& result = *reinterpret_cast<const Data*>(buffer.data());

    EXPECT_EQ(result.v1, expected.v1);
    EXPECT_EQ(result.v2, expected.v2);
    EXPECT_EQ(result.v3, expected.v3);
    EXPECT_EQ(result.v4, expected.v4);
}

TEST(Buffer, BufferRead)
{
    const int8_t expected1 = 0x41;
    const int16_t expected2 = 0x6433;
    const int32_t expected3 = 0x63415431;
    const int64_t expected4 = 0x1234567887654321;

    // arrange
    Buffer buffer = []()
        {
            std::vector<char> buffer;
            zerosugar::StreamWriter bufferWriter(buffer);

            bufferWriter.Write(expected1);
            bufferWriter.Write(expected2);
            bufferWriter.Write(expected3);
            bufferWriter.Write(expected4);

            Buffer result;
            result.Add(Fragment::CreateFrom(buffer));

            return result;
        }();

    BufferReader reader(buffer.cbegin(), buffer.cend());

    // act

    const int8_t result1 = reader.Read<int8_t>();
    const int16_t result2 = reader.Read<int16_t>();
    const int32_t result3 = reader.Read<int32_t>();
    const int64_t result4 = reader.Read<int64_t>();

    // assert

    EXPECT_EQ(result1, expected1);
    EXPECT_EQ(result2, expected2);
    EXPECT_EQ(result3, expected3);
    EXPECT_EQ(result4, expected4);
    EXPECT_ANY_THROW(reader.Read<int8_t>());
}

TEST(Buffer, BufferReadWriteString)
{
    const std::string input1 = "fians1234";
    const std::string input2 = "gj24tnsdf";
    constexpr int64_t input3 = 0x832751912385134;

    // arrange
    Buffer buffer = [&]()
        {
            std::vector<char> buffer;
            zerosugar::StreamWriter bufferWriter(buffer);

            bufferWriter.WriteString(input1);
            bufferWriter.WriteString(input2);
            bufferWriter.Write(input3);

            Buffer result;
            result.Add(Fragment::CreateFrom(buffer));

            return result;
        }();

    BufferReader reader(buffer.cbegin(), buffer.cend());

    // act
    const std::string& result1 = reader.ReadString();
    const std::string& result2 = reader.ReadString();
    const int64_t result3 = reader.Read<int64_t>();

    // assert
    EXPECT_EQ(result1, input1);
    EXPECT_EQ(result2, input2);
    EXPECT_EQ(result3, input3);
}
