#pragma once

namespace zerosugar::sl
{
    class PacketWriter;

    class IPacketSerializable
    {
    public:
        virtual ~IPacketSerializable() = default;

        virtual void Serialize(PacketWriter& writer) const = 0;
    };
}
