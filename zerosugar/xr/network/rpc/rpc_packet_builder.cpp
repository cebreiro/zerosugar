#include "rpc_packet_builder.h"

#include "zerosugar/xr/network/packet_writer.h"
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/network/model/generated/rpc_generated.h"

namespace zerosugar::xr
{
    auto RPCPacketBuilder::MakePacket(const IPacket& packet) -> Buffer
    {
        PacketWriter packetWriter;

        packetWriter.Write<int32_t>(packet.GetOpcode());
        packet.Serialize(packetWriter);

        const int64_t packetSize = packetWriter.GetWriteSize();
        const int64_t size = packetSize + 4;
        assert(size >= 8);

        Buffer buffer;
        buffer.Add(buffer::Fragment::Create(size));

        BufferWriter bufferWriter(buffer);
        bufferWriter.Write<int32_t>(static_cast<int32_t>(size));
        bufferWriter.WriteBuffer(std::span(packetWriter.GetBuffer().data(), packetSize));

        return buffer;
    }
}
