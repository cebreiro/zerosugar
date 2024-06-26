#pragma once

namespace zerosugar::xr
{
    class IPacket;

    class Packet
    {
    public:
        Packet() = delete;

        static auto ToBuffer(const IPacket& packet) -> Buffer;
    };
}
