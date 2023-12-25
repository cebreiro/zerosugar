#pragma once

namespace zerosugar
{
    class BufferReader;

    class IBufferDeserializable
    {
    public:
        virtual ~IBufferDeserializable() = default;

        virtual void Deserialize(BufferReader& reader) = 0;
    };
}
