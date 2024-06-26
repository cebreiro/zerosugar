#include "buffer_writer.h"

#include "zerosugar/shared/network/buffer/buffer_serializable.h"

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
            if (_iter == _end)
            {
                _buffer.Add(buffer::Fragment::Create(256));

                auto end = _buffer.GetFragmentContainer().end();

                _iter = Buffer::iterator(std::prev(end), end, 0);
            }

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
        , _adapter(_buffer)
        , _streamWriter(_adapter)
    {
    }

    void BufferWriter::WriteString(const std::string& str)
    {
        _streamWriter.WriteString(str);
    }

    void BufferWriter::WriteBuffer(std::span<const char> buffer)
    {
        _streamWriter.WriteBuffer(buffer);
    }

    void BufferWriter::Write(const IBufferSerializable& serializable)
    {
        serializable.Serialize(*this);
    }

    auto BufferWriter::GetWriteSize() const -> int64_t
    {
        return _streamWriter.GetWriteSize();
    }
}
