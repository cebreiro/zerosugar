#pragma once

namespace zerosugar::xr
{
    class IPacket;

    class RPCPacket
    {
    public:
        RPCPacket() = delete;

        static auto ToBuffer(const IPacket& packet) -> Buffer;
    };
}
