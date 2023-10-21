#include "buffer_reader.h"

#include "zerosugar/shared/network/buffer/buffer_readable.h"

namespace zerosugar
{
    BufferReader::BufferReader(const value_type& begin, const value_type& end)
        : BasicStreamReader(begin, end)
    {
    }

    void BufferReader::Read(buffer::IBufferReadable& readable)
    {
        readable.Read(*this);
    }
}
