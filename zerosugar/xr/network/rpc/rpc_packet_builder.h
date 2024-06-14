#pragma once

namespace zerosugar::xr
{
    class IPacket;

    class RPCPacketBuilder
    {
    public:
        RPCPacketBuilder() = delete;

        static auto MakePacket(const IPacket& packet) -> Buffer;
    };
}