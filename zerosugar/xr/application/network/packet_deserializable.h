#pragma once

namespace zerosugar::xr
{
    class PacketReader;

    class IPacketDeserializable
    {
    public:
        virtual ~IPacketDeserializable() = default;

        virtual void Deserialize(PacketReader& reader) = 0;
    };
}
