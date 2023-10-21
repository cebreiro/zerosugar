#pragma once

namespace zerosugar
{
    class BufferReader;
}

namespace zerosugar::buffer
{
    class IBufferReadable
    {
    public:
        virtual ~IBufferReadable() = default;

        virtual void Read(BufferReader& reader) = 0;
    };
}
