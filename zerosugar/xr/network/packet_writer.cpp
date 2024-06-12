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

    auto PacketWriter::GetBuffer() const -> std::span<const char>
    {
        return _buffer;
    }

    auto PacketWriter::GetWriteSize() const -> int64_t
    {
        return std::ssize(_buffer);
    }
}
