#pragma once

namespace zerosugar
{
    class BufferWriter;

    class IBufferSerializable
    {
    public:
        virtual ~IBufferSerializable() = default;

        virtual void Serialize(BufferWriter& writer) const = 0;
    };
}
