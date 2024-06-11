#include "service_packet_builder.h"

#include "zerosugar/xr/network/packet_writer.h"
#include "zerosugar/xr/network/model/generated/service_to_service_generated.h"

namespace zerosugar::xr
{
    auto ServicePacketBuilder::MakePacket(const network::service::RequestRemoteProcedureCall& item) -> Buffer
    {
        PacketWriter writer;

        writer.Write<int32_t>(network::service::RequestRemoteProcedureCall::opcode);
        item.Serialize(writer);

        return writer.MakeBuffer();
    }

    auto ServicePacketBuilder::MakePacket(const network::service::ResultRemoteProcedureCall& item) -> Buffer
    {
        PacketWriter writer;

        writer.Write<int32_t>(network::service::ResultRemoteProcedureCall::opcode);
        item.Serialize(writer);

        return writer.MakeBuffer();
    }
}
