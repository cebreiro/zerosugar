#include "buffer_reader.h"

#include "zerosugar/shared/network/buffer/buffer_deserializable.h"

namespace zerosugar
{
    BufferReader::BufferReader(const value_type& begin, const value_type& end)
        : StreamReader(begin, end)
    {
    }

    void BufferReader::Read(IBufferDeserializable& deserializable)
    {
        deserializable.Deserialize(*this);
    }
}
