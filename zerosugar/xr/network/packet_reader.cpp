#include "packet_reader.h"

namespace zerosugar::xr
{
    PacketReader::PacketReader(Buffer::const_iterator begin, Buffer::const_iterator end)
        : _bufferReader(begin, end)
    {
    }

    auto PacketReader::ReadString() -> std::string
    {
        const auto size = Read<int16_t>();

        return _bufferReader.ReadString(size);
    }

    void PacketReader::ReadBytes(std::span<char> buffer, int64_t size)
    {
        assert(std::ssize(buffer) >= size);

        _bufferReader.ReadBuffer(buffer.data(), size);
    }

    auto PacketReader::GetReadSize() const -> int64_t
    {
        return _bufferReader.GetReadSize();
    }
}
