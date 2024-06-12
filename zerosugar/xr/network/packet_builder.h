#pragma once

namespace zerosugar::xr
{
    class IPacket;

    class PacketBuilder
    {
    public:
        PacketBuilder() = delete;

        static auto MakePacket(const IPacket& packet) -> Buffer;
    };
}
