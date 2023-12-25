#include "packet_writer.h"

#include "zerosugar/shared/network/buffer/buffer_serializable.h"
#include "zerosugar/sl/protocol/packet/packet_serializable.h"

namespace zerosugar::sl
{
    void PacketWriter::WriteString(std::string_view str)
    {
        block_t& block = _blocks.emplace_back();

        uint16_t size = static_cast<uint16_t>(std::ssize(str) + 1);
        if (!str.empty() && str.back() == '\0')
        {
            size -= 1;
        }

        if (size <= 0x6E)
        {
            block.push_back(8ui8 + static_cast<uint8_t>(size));
        }
        else
        {
            block.push_back(detail::PacketValueTraits<std::string>::type_value);
            std::copy_n(reinterpret_cast<const uint8_t*>(&size), sizeof(uint16_t), std::back_inserter(block));
        }

        std::copy_n(str.data(), size, std::back_inserter(block));

        _size += std::ssize(block);
        assert(_size == CalculateSize());
    }

    void PacketWriter::WriteObject(std::span<const char> buffer)
    {
        block_t& block = _blocks.emplace_back();

        const uint16_t size = static_cast<uint16_t>(std::ssize(buffer));

        if (size <= 0x6E)
        {
            block.push_back(0x81ui8 + static_cast<uint8_t>(size));
        }
        else
        {
            block.push_back(detail::PacketValueTraits<detail::object_type>::type_value);
            std::copy_n(reinterpret_cast<const uint8_t*>(&size), sizeof(uint16_t), std::back_inserter(block));
        }

        std::copy_n(buffer.begin(), size, std::back_inserter(block));

        _size += std::ssize(block);
        assert(_size == CalculateSize());
    }

    void PacketWriter::WriteObject(const IBufferSerializable& object)
    {
        Buffer buffer;
        BufferWriter writer(buffer);
        object.Serialize(writer);

        block_t& block = _blocks.emplace_back();

        const uint16_t size = static_cast<uint16_t>(buffer.GetSize());

        if (size <= 0x6E)
        {
            block.push_back(0x81ui8 + static_cast<uint8_t>(size));
        }
        else
        {
            block.push_back(detail::PacketValueTraits<detail::object_type>::type_value);
            std::copy_n(reinterpret_cast<const uint8_t*>(&size), sizeof(uint16_t), std::back_inserter(block));
        }

        std::copy_n(buffer.begin(), size, std::back_inserter(block));

        _size += std::ssize(block);
        assert(_size == CalculateSize());
    }

    void PacketWriter::WriteZero(int64_t length)
    {
        block_t& block = _blocks.emplace_back();

        if (length <= 0x6E)
        {
            block.resize(length + 1, 0);
            block[0] = static_cast<uint8_t>(0x81ui8 + length);
        }
        else
        {
            block.resize(length + 3, 0);
            block[0] = detail::PacketValueTraits<detail::object_type>::type_value;
            *reinterpret_cast<uint16_t*>(block.data() + 1) = static_cast<uint16_t>(length);
        }

        _size += std::ssize(block);
        assert(_size == CalculateSize());
    }

    void PacketWriter::Write(const IPacketSerializable& serializable)
    {
        serializable.Serialize(*this);
    }

    auto PacketWriter::MakePacket() const -> Buffer
    {
        if (_size <= 0)
        {
            return {};
        }

        Buffer packet = MakeHeader();
        packet.MergeBack(MakeBody());

        return packet;
    }

    auto PacketWriter::GetSize() const -> int64_t
    {
        assert(_size == CalculateSize());

        return _size;
    }

    auto PacketWriter::CalculateSize() const -> int64_t
    {
        return std::accumulate(_blocks.begin(), _blocks.end(), int64_t{ 0 },
            [](int64_t sum, const block_t& block)
            {
                return sum + std::ssize(block);
            });
    }

    auto PacketWriter::MakeHeader() const -> Buffer
    {
        constexpr uint32_t headerSize = 13;
        const uint32_t bodySize = static_cast<uint32_t>(_size);
        const uint32_t packetSize = headerSize + bodySize;

        Buffer buffer;
        buffer.Add(buffer::Fragment::Create(headerSize));

        BufferWriter writer(buffer);
        writer.Write<uint32_t>(packetSize);
        writer.Write<uint32_t>(packetSize - 9);
        writer.Write<uint8_t>(static_cast<uint8_t>((packetSize & 0xFF) + (packetSize / 0xFF)));
        writer.Write<uint16_t>(static_cast<uint16_t>((packetSize - 9) & 0xFFFF));
        writer.Write<uint16_t>(static_cast<uint16_t>(_blocks.size()));

        assert(buffer.GetSize() == headerSize);

        return buffer;
    }

    auto PacketWriter::MakeBody() const -> Buffer
    {
        Buffer buffer;
        buffer.Add(buffer::Fragment::Create(_size));

        auto range = _blocks | std::views::reverse | std::views::join;
        assert(std::ranges::distance(range) == buffer.GetSize());

        std::ranges::copy_n(range.begin(), buffer.GetSize(), buffer.begin());
        return buffer;
    }
}
