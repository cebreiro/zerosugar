#include "packet_writer.h"

namespace zerosugar::xr
{
    void PacketWriter::Write(std::string_view str)
    {
        Write<int64_t>(static_cast<int64_t>(str.size()));
        WriteBytes(std::span(str.data(), str.size()));
    }

    void PacketWriter::WriteBytes(std::span<const char> buffer)
    {
        std::copy_n(buffer.begin(), buffer.size(), std::back_inserter(_buffer));
    }

    auto PacketWriter::GetWriteSize() const -> int64_t
    {
        return std::ssize(_buffer);
    }

    auto PacketWriter::MakeBuffer() const -> Buffer
    {
        const int64_t size = GetWriteSize() + 4;
        assert(size > 4);

        Buffer buffer;
        buffer.Add(buffer::Fragment::Create(size));

        BufferWriter writer(buffer);
        writer.Write<int32_t>(static_cast<int32_t>(size));
        writer.WriteBuffer(std::span(_buffer.data() + 4, GetWriteSize() - 4));

        return buffer;
    }
}
