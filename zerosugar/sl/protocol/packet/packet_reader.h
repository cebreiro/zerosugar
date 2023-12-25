#pragma once
#include "zerosugar/shared/network/buffer/buffer_reader.h"
#include "zerosugar/sl/protocol/packet/packet_value_traits.h"

namespace zerosugar::sl
{
    class IPacketDeserializable;

    class PacketReader
    {
    public:
        static constexpr int64_t packet_header_size = 13;

        struct ParseHeaderResult
        {
            enum class ErrorCode
            {
                None = 0,
                ShortLength = 1,
                InvalidFormat = 2,
            };

            ErrorCode errorCode = ErrorCode::None;
            int64_t packetSize = 0;
        };

    public:
        PacketReader() = delete;
        explicit PacketReader(const Buffer& buffer);

        template <packet_primitive_value_concept T>
        auto Read() -> T;

        template <packet_primitive_value_concept T>
        auto Peek() -> T;

        auto ReadInt64() -> std::pair<int32_t, int32_t>;
        auto ReadUInt64() -> std::pair<uint32_t, uint32_t>;
        auto ReadString() -> std::string;

        template <typename T> requires std::is_base_of_v<IBufferDeserializable, T>
        auto Read() -> T;
        auto ReadObject() -> BufferReader;
        void ReadObject(std::vector<char>& outBuffer);
        void Read(IPacketDeserializable& deserializable);

        static auto ParseHeader(const Buffer& buffer) -> ParseHeaderResult;

    private:
        void Initialize(const Buffer& buffer);

        static void ThrowIfInvalidRead(uint8_t expected, uint8_t result);

    private:
        using buffer_reader_container_type = boost::container::small_vector<BufferReader, 16>;
        buffer_reader_container_type _readers;
        int64_t _index = 0;
    };

    template <packet_primitive_value_concept T>
    auto PacketReader::Read() -> T
    {
        BufferReader& reader = _readers.at(_index);
        ++_index;

        constexpr uint8_t expected = detail::PacketValueTraits<T>::type_value;
        ThrowIfInvalidRead(expected, reader.Read<uint8_t>());

        return reader.Read<T>();
    }

    template <packet_primitive_value_concept T>
    auto PacketReader::Peek() -> T
    {
        BufferReader reader = _readers.at(_index);

        constexpr uint8_t expected = detail::PacketValueTraits<T>::type_value;
        ThrowIfInvalidRead(expected, reader.Read<uint8_t>());

        return reader.Read<T>();
    }

    template <typename T> requires std::is_base_of_v<IBufferDeserializable, T>
    auto PacketReader::Read() -> T
    {
        T object = {};
        ReadObject().Read(object);

        return object;
    }
}
