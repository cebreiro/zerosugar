#include "packet_writer.h"

#include "zerosugar/shared/network/buffer/buffer_serializable.h"

namespace zerosugar::xr
{
    void PacketWriter::Write(std::string_view str)
    {
        Write<int16_t>(static_cast<int16_t>(str.size()));

        if (str.empty())
        {
            return;
        }

        WriteBytes(std::span(str.data(), str.size()));
    }

    void PacketWriter::WriteBytes(std::span<const char> buffer)
    {
        if (buffer.empty())
        {
            return;
        }

        std::copy_n(buffer.begin(), buffer.size(), std::back_inserter(_buffer));
    }

    void PacketWriter::Write(const IBufferSerializable& object)
    {
        WriteObject(object);
    }

    void PacketWriter::WriteObject(const IBufferSerializable& object)
    {
        // TODO: optimize
        Buffer buffer;
        buffer.Add(buffer::Fragment::Create(256));

        BufferWriter writer(buffer);
        object.Serialize(writer);

        const int64_t size = writer.GetWriteSize();

        std::copy_n(buffer.begin(), size, std::back_inserter(_buffer));
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
