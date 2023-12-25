#include "packet_reader.h"

#include "zerosugar/sl/protocol/packet/packet_deserializable.h"

namespace zerosugar::sl
{
    PacketReader::PacketReader(const Buffer& buffer)
    {
        Initialize(buffer);
    }

    auto PacketReader::ReadInt64() -> std::pair<int32_t, int32_t>
    {
        BufferReader& reader = _readers.at(_index);
        ++_index;

        constexpr uint8_t expected = 0x81ui8 + 8;
        ThrowIfInvalidRead(expected, reader.Read<uint8_t>());

        const int32_t low = reader.Read<int32_t>();
        const int32_t high = reader.Read<int32_t>();

        return std::make_pair(low, high);
    }

    auto PacketReader::ReadUInt64() -> std::pair<uint32_t, uint32_t>
    {
        BufferReader& reader = _readers.at(_index);
        ++_index;

        constexpr uint8_t expected = 0x81ui8 + 8;
        ThrowIfInvalidRead(expected, reader.Read<uint8_t>());

        const uint32_t low = reader.Read<uint32_t>();
        const uint32_t high = reader.Read<uint32_t>();

        return std::make_pair(low, high);
    }

    auto PacketReader::ReadString() -> std::string
    {
        BufferReader& reader = _readers.at(_index);
        ++_index;

        const uint8_t type = reader.Read<uint8_t>();
        const uint16_t size = type == detail::PacketValueTraits<std::string>::type_value
            ? reader.Read<uint16_t>() : type - 8ui8;

        return reader.ReadString(size);
    }

    auto PacketReader::ReadObject() -> BufferReader
    {
        BufferReader& reader = _readers.at(_index);
        ++_index;

        return BufferReader(std::move(reader));
    }

    void PacketReader::ReadObject(std::vector<char>& outBuffer)
    {
        const BufferReader reader = ReadObject();
        const auto begin = reader.begin();
        const auto end = reader.end();

        outBuffer.reserve(std::distance(begin, end));
        std::copy(begin, end, std::back_inserter(outBuffer));
    }

    void PacketReader::Read(IPacketDeserializable& deserializable)
    {
        deserializable.Deserialize(*this);
    }

    auto PacketReader::ParseHeader(const Buffer& buffer) ->ParseHeaderResult
    {
        if (buffer.GetSize() < packet_header_size)
        {
            return ParseHeaderResult{
                .errorCode = ParseHeaderResult::ErrorCode::ShortLength,
            };
        }

        BufferReader reader(buffer.begin(), buffer.end());

        const uint32_t size = reader.Read<uint32_t>();
        if (buffer.GetSize() < size)
        {
            return ParseHeaderResult{
                .errorCode = ParseHeaderResult::ErrorCode::ShortLength,
            };
        }

        if (size < packet_header_size || size >= std::numeric_limits<uint16_t>::max() - packet_header_size)
        {
            return ParseHeaderResult{
                .errorCode = ParseHeaderResult::ErrorCode::InvalidFormat,
            };
        }

        if (reader.Read<uint32_t>() != (size - 9))
        {
            return ParseHeaderResult{
                .errorCode = ParseHeaderResult::ErrorCode::InvalidFormat,
            };
        }

        if (reader.Read<uint8_t>() != (size & 0xFF) + (size / 0xFF))
        {
            return ParseHeaderResult{
                .errorCode = ParseHeaderResult::ErrorCode::InvalidFormat,
            };
        }

        if (reader.Read<uint16_t>() != ((size - 9) & 0xFFFF))
        {
            return ParseHeaderResult{
                .errorCode = ParseHeaderResult::ErrorCode::InvalidFormat,
            };
        }

        return ParseHeaderResult{
            .errorCode = ParseHeaderResult::ErrorCode::None,
            .packetSize = size,
        };
    }

    void PacketReader::Initialize(const Buffer& buffer)
    {
        const auto end = buffer.end();
        BufferReader reader(std::next(buffer.begin(), 11), end);

        const size_t blockCount = reader.Read<uint16_t>();

        // client 58DA70h
        for (size_t i = 0; i < blockCount; ++i)
        {
            const Buffer::const_iterator current = reader.current();
            _readers.push_back(BufferReader(current, end));

            const uint8_t typeValue = reader.Read<uint8_t>();
            if (typeValue == detail::PacketValueTraits<int8_t>::type_value)
            {
                reader.Skip(sizeof(int8_t));
            }
            else if (typeValue == detail::PacketValueTraits<int16_t>::type_value)
            {
                reader.Skip(sizeof(int16_t));
            }
            else if (typeValue == detail::PacketValueTraits<int32_t>::type_value)
            {
                reader.Skip(sizeof(int32_t));
            }
            else if (typeValue == detail::PacketValueTraits<float>::type_value)
            {
                reader.Skip(sizeof(float));
            }
            else if (typeValue == detail::PacketValueTraits<std::string>::type_value ||
                typeValue == detail::PacketValueTraits<detail::object_type>::type_value)
            {
                reader.Skip(reader.Read<uint16_t>());
            }
            else
            {
                if (typeValue > 0x81 && typeValue <= 0xFF)
                {
                    reader.Skip(typeValue - 0x81ui8);
                }
                else if (typeValue > 8 && typeValue <= 0x6E)
                {
                    reader.Skip(typeValue - 8ui8);
                }
                else
                {
                    throw std::runtime_error(
                        std::format("packet_reader found unknown type: {}, buffer: {}",
                            typeValue, buffer.ToString()));
                }
            }
        }

        if (reader.current() != end)
        {
            throw std::runtime_error(
                std::format("packet_reader parse error. buffer: {}", buffer.ToString()));
        }

        std::ranges::reverse(_readers);
    }

    void PacketReader::ThrowIfInvalidRead(uint8_t expected, uint8_t result)
    {
        if (expected != result)
        {
            assert(false);
            throw std::runtime_error("invalid packet read");
        }
    }
}
