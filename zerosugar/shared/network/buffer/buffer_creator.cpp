#include "buffer_creator.h"

#include "zerosugar/shared/network/buffer/buffer_writable.h"

namespace zerosugar
{
    BufferCreator::BufferCreator()
    {
        BasicStreamWriter::_iter = std::back_inserter(_buffer);
    }

    void BufferCreator::Write(const buffer::IBufferWritable& writable)
    {
        writable.Write(*this);
    }

    auto BufferCreator::CreateBuffer() const -> Buffer
    {
        if (_buffer.empty())
        {
            return Buffer{};
        }

        Buffer result;
        result.Add(buffer::Fragment::CreateFrom(_buffer));

        return result;
    }
}
