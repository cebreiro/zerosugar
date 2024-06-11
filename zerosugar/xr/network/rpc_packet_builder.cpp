#include "rpc_packet_builder.h"

#include "zerosugar/xr/network/packet_writer.h"
#include "zerosugar/xr/network/model/generated/rpc_generated.h"

namespace zerosugar::xr
{
    auto RPCPacketBuilder::MakePacket(const network::RequestRemoteProcedureCall& item) -> Buffer
    {
        PacketWriter writer;

        writer.Write<int32_t>(network::RequestRemoteProcedureCall::opcode);
        item.Serialize(writer);

        return writer.MakeBuffer();
    }

    auto RPCPacketBuilder::MakePacket(const network::ResultRemoteProcedureCall& item) -> Buffer
    {
        PacketWriter writer;

        writer.Write<int32_t>(network::ResultRemoteProcedureCall::opcode);
        item.Serialize(writer);

        return writer.MakeBuffer();
    }
}
