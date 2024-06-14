#include "rpc_packet_builder.h"

#include "zerosugar/xr/network/packet_writer.h"
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/network/model/generated/rpc_message.h"

namespace zerosugar::xr
{
    auto RPCPacketBuilder::MakePacket(const IPacket& packet) -> Buffer
    {
        using length_type = int32_t;
        using opcode_type = int32_t;

        PacketWriter packetWriter;
        packet.Serialize(packetWriter);

        const int64_t writeSize = packetWriter.GetWriteSize();
        const int64_t packetSize = sizeof(length_type) + sizeof(opcode_type) + writeSize;

        Buffer buffer;
        buffer.Add(buffer::Fragment::Create(packetSize));

        BufferWriter bufferWriter(buffer);
        bufferWriter.Write<length_type>(static_cast<length_type>(packetSize));
        bufferWriter.Write<opcode_type>(static_cast<opcode_type>(packet.GetOpcode()));
        bufferWriter.WriteBuffer(std::span(packetWriter.GetBuffer().data(), writeSize));

        return buffer;
    }
}
