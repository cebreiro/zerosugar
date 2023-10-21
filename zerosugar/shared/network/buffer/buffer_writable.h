#pragma once

namespace zerosugar
{
    class BufferCreator;
}

namespace zerosugar::buffer
{
    class IBufferWritable
    {
    public:
        virtual ~IBufferWritable() = default;

        virtual void Write(BufferCreator& writer) const = 0;
    };
}
