#pragma once
#include "zerosugar/shared/io/stream_reader.h"
#include "zerosugar/shared/network/buffer/buffer.h"

namespace zerosugar
{
    namespace buffer
    {
        class IBufferReadable;
    }

    class BufferReader : public BasicStreamReader<Buffer::const_iterator>
    {
    public:
        using BasicStreamReader::Read;

    public:
        BufferReader(const value_type& begin, const value_type& end);

        void Read(buffer::IBufferReadable& readable);
    };
}
