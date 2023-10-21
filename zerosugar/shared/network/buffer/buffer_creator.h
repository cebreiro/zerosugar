#pragma once
#include <boost/container/small_vector.hpp>
#include "zerosugar/shared/io/stream_writer.h"
#include "zerosugar/shared/network/buffer/buffer.h"

namespace zerosugar
{
    namespace buffer
    {
        static constexpr size_t base_buffer_size = 256;
        using internal_buffer_type = boost::container::small_vector<char, base_buffer_size>;

        class IBufferWritable;
    }

    class BufferCreator : public BasicStreamWriter<buffer::internal_buffer_type>
    {
    public:
        using BasicStreamWriter::Write;

    public:
        BufferCreator();

        void Write(const buffer::IBufferWritable& writable);

        auto CreateBuffer() const -> Buffer;

    private:
        buffer::internal_buffer_type _buffer;
    };
}
