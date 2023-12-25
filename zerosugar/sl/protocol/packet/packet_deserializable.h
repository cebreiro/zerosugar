#pragma once

namespace zerosugar::sl
{
    class PacketReader;

    class IPacketDeserializable
    {
    public:
        virtual ~IPacketDeserializable() = default;

        virtual void Deserialize(PacketReader& reader) = 0;
    };
}
