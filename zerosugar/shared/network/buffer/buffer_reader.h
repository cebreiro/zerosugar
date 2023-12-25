#pragma once
#include "zerosugar/shared/io/stream_reader.h"
#include "zerosugar/shared/network/buffer/buffer.h"

namespace zerosugar
{
    class IBufferDeserializable;

    class BufferReader : public StreamReader<Buffer::const_iterator>
    {
    public:
        using StreamReader::Read;

    public:
        BufferReader() = default;
        BufferReader(const value_type& begin, const value_type& end);

        void Read(IBufferDeserializable& deserializable);
    };
}
