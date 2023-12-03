#include "buffer_writer.h"

namespace zerosugar
{
    namespace detail
    {
        BufferStreamAdapter::BufferStreamAdapter(Buffer& buffer)
            : _buffer(buffer)
            , _begin(buffer.begin())
            , _iter(_begin)
            , _end(buffer.end())
        {
        }

        void BufferStreamAdapter::push_back(value_type c)
        {
            assert(_iter != _end);

            *_iter = c;
            ++_iter;
        }

        auto BufferStreamAdapter::size() const -> size_t
        {
            return _buffer.GetSize();
        }

        auto BufferStreamAdapter::begin() -> Buffer::iterator
        {
            return _buffer.begin();
        }
    }

    BufferWriter::BufferWriter(Buffer& buffer)
        : _buffer(buffer)
        , _remainSize(buffer.GetSize())
        , _adapter(_buffer)
        , _streamWriter(_adapter)
    {
    }

    void BufferWriter::WriteString(const std::string& str)
    {
        ExpandIfNoSpace(str.size());

        _streamWriter.WriteString(str);
    }

    void BufferWriter::WriteBuffer(std::span<const char> buffer)
    {
        ExpandIfNoSpace(std::ssize(buffer));

        _streamWriter.WriteBuffer(buffer);
    }

    void BufferWriter::ExpandIfNoSpace(int64_t requiredSize)
    {
        if (_remainSize >= requiredSize)
        {
            return;
        }

        Expand(requiredSize - _remainSize);
        assert(_remainSize > requiredSize);
    }

    void BufferWriter::Expand(int64_t minRequiredSize)
    {
        const int64_t size = minRequiredSize + 1 & ~1;
        assert(size > minRequiredSize);

        _buffer.Add(buffer::Fragment(std::make_shared<char[]>(size), 0, size));
        _remainSize += size;
    }
}
