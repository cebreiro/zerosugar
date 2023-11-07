#include "buffer_reader.h"

#include "zerosugar/shared/network/buffer/buffer_readable.h"

namespace zerosugar
{
    BufferReader::BufferReader(value_type begin, value_type end)
        : StreamReader(std::move(begin), std::move(end))
    {
    }

    void BufferReader::Read(buffer::IBufferReadable& readable)
    {
        readable.Read(*this);
    }
}
