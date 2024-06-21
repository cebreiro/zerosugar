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

        template <typename T> requires std::is_enum_v<T>
        void Read(T& value)
        {
            value = static_cast<T>(Read<std::underlying_type_t<T>>());
        }

        template <typename T>
        void Read(T& value, int64_t count)
        {
            value.reserve(count);

            for (int64_t i = 0; i < count; ++i)
            {
                if constexpr (std::derived_from<typename T::value_type, IBufferDeserializable>)
                {
                    typename T::value_type item;
                    item.Deserialize(*this);

                    value.push_back(item);
                }
                else
                {
                    value.push_back(Read<typename T::value_type>());
                }
            }
        }
    };
}
