#pragma once
#include "zerosugar/sl/protocol/packet/packet_value_traits.h"

namespace zerosugar
{
    class IBufferSerializable;
}

namespace zerosugar::sl
{
    class IPacketSerializable;

    class PacketWriter
    {
    public:
        template <packet_primitive_value_concept T>
        void Write(T value);

        void WriteString(std::string_view str);
        void WriteObject(std::span<const char> buffer);
        void WriteObject(const IBufferSerializable& object);
        void WriteZero(int64_t length);
        void Write(const IPacketSerializable& serializable);

        auto MakePacket() const -> Buffer;

        auto GetSize() const -> int64_t;

    private:
        auto CalculateSize() const -> int64_t;

        auto MakeHeader() const -> Buffer;
        auto MakeBody() const -> Buffer;

    private:
        using block_t = boost::container::small_vector<char, 64>;
        boost::container::small_vector<block_t, 16> _blocks;

        int64_t _size = 0;
    };

    template <packet_primitive_value_concept T>
    void PacketWriter::Write(T value)
    {
        block_t& block = _blocks.emplace_back();

        std::array<uint8_t, sizeof(T) + 1> buffer = {};
        buffer[0] = detail::PacketValueTraits<T>::type_value;
        *reinterpret_cast<T*>(buffer.data() + 1) = value;

        std::copy_n(buffer.begin(), sizeof(buffer), std::back_inserter(block));

        _size += std::ssize(block);
        assert(_size == CalculateSize());
    }
}
