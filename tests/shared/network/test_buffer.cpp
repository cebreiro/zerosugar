#include <boost/container/small_vector.hpp>
#include "zerosugar/shared/io/stream_writer.h"
#include "zerosugar/shared/network/buffer/buffer.h"
#include "zerosugar/shared/network/buffer/buffer_reader.h"

using zerosugar::Buffer;
using zerosugar::buffer::Fragment;
using zerosugar::BufferReader;

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

TEST(Buffer, Merge)
{
    // arrange
    constexpr int64_t size = 16;
    constexpr int64_t count = 3;

    Buffer buffer1;
    for (int64_t i = 0; i < count; ++i)
    {
        buffer1.Add(Fragment::Create(size));
    }

    Buffer buffer2;
    buffer2.Add(Fragment::Create(size));

    // act
    buffer2.MergeBack(std::move(buffer1));

    // assert
    EXPECT_EQ(buffer1.GetSize(), 0);
    EXPECT_EQ(std::distance(buffer1.begin(), buffer1.end()), buffer1.GetSize());

    EXPECT_EQ(buffer2.GetSize(), size * (count + 1));
    EXPECT_EQ(std::distance(buffer2.begin(), buffer2.end()), buffer2.GetSize());
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
