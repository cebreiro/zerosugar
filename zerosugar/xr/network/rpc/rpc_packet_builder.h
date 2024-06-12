#pragma once
#include "zerosugar/xr/network/packet_serializable.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr
{
    class RPCPacketBuilder
    {
    public:
        RPCPacketBuilder() = delete;

        template <typename T> requires std::derived_from<T, IPacketSerializable>
        static auto MakePacket(const T& item) -> Buffer
        {
            PacketWriter writer;

            writer.Write<int32_t>(network::ResultRemoteProcedureCall::opcode);
            item.Serialize(writer);

            return writer.MakeBuffer();
        }
    };
}
