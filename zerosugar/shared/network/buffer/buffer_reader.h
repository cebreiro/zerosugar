#pragma once
#include "zerosugar/shared/io/stream_reader.h"
#include "zerosugar/shared/network/buffer/buffer.h"

namespace zerosugar
{
    namespace buffer
    {
        class IBufferReadable;
    }

    class BufferReader : public StreamReader<Buffer::const_iterator>
    {
    public:
        using StreamReader::Read;

    public:
        BufferReader(value_type begin, value_type end);

        void Read(buffer::IBufferReadable& readable);
    };
}
