#pragma once
#include "zerosugar/shared/io/stream_writer.h"
#include "zerosugar/shared/network/buffer/buffer.h"

namespace zerosugar
{
    namespace detail
    {
        class BufferStreamAdapter
        {
        public:
            using value_type = char;

        public:
            BufferStreamAdapter(Buffer& buffer);

            void push_back(value_type c);
            auto size() const -> size_t;

            auto begin() -> Buffer::iterator;

        private:
            Buffer& _buffer;
            Buffer::iterator _begin;
            Buffer::iterator _iter;
            Buffer::iterator _end;
        };
    }

    class IBufferSerializable;

    // TODO: make test
    class BufferWriter
    {
    public:
        BufferWriter() = delete;
        BufferWriter(const BufferWriter& other) = delete;
        BufferWriter(BufferWriter&& other) noexcept = delete;
        BufferWriter& operator=(const BufferWriter& other) = delete;
        BufferWriter& operator=(BufferWriter&& other) noexcept = delete;

        explicit BufferWriter(Buffer& buffer);

        template <std::integral U>
        void Write(U value);
        void WriteString(const std::string& str);
        void WriteBuffer(std::span<const char> buffer);

        template <std::integral U>
        void Write(U value, int64_t offset);

        void Write(const IBufferSerializable& serializable);

        template <typename T> requires std::is_enum_v<T>
        void Write(T value);

        auto GetWriteSize() const -> int64_t;

    private:
        void ExpandIfNoSpace(int64_t requiredSize);
        void Expand(int64_t minRequiredSize);

    private:
        Buffer& _buffer;
        int64_t _remainSize = 0;
        detail::BufferStreamAdapter _adapter;
        StreamWriter<detail::BufferStreamAdapter> _streamWriter;
    };

    template <std::integral U>
    void BufferWriter::Write(U value)
    {
        ExpandIfNoSpace(sizeof(value));

        _streamWriter.Write(value);
    }

    template <std::integral U>
    void BufferWriter::Write(U value, int64_t offset)
    {
        _streamWriter.Write(value, offset);
    }

    template <typename T> requires std::is_enum_v<T>
    void BufferWriter::Write(T value)
    {
        Write<std::underlying_type_t<T>>(value);
    }
}
