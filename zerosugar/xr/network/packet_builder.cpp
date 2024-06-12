#include "packet_builder.h"

#include "zerosugar/xr/network/packet_writer.h"
#include "zerosugar/xr/network/packet_interface.h"

namespace zerosugar::xr
{
    auto PacketBuilder::MakePacket(const IPacket& packet) -> Buffer
    {
        PacketWriter packetWriter;

        packetWriter.Write<int16_t>(static_cast<int16_t>(packet.GetOpcode()));
        packet.Serialize(packetWriter);

        const int64_t packetSize = packetWriter.GetWriteSize();
        const int64_t size = packetSize + 2;
        assert(size >= 6);

        Buffer buffer;
        buffer.Add(buffer::Fragment::Create(size));

        BufferWriter bufferWriter(buffer);
        bufferWriter.Write<int16_t>(static_cast<int16_t>(size));
        bufferWriter.WriteBuffer(std::span(packetWriter.GetBuffer().data(), packetSize));

        return buffer;
    }
}
