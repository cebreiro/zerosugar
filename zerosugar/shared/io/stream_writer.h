#pragma once
#include <cassert>
#include <span>
#include <concepts>
#include <string>
#include <iterator>
#include <algorithm>
#include <optional>

namespace zerosugar
{
    template <typename T>
    concept stream_writable_concept = requires (T t, typename T::value_type v)
    {
        { t.push_back(v) } -> std::same_as<void>;
    };

    template <stream_writable_concept T>
    class BasicStreamWriter
    {
    public:
        BasicStreamWriter() = default;
        BasicStreamWriter(const BasicStreamWriter& other) = delete;
        BasicStreamWriter(BasicStreamWriter&& other) noexcept = delete;
        BasicStreamWriter& operator=(const BasicStreamWriter& other) = delete;
        BasicStreamWriter& operator=(BasicStreamWriter&& other) noexcept = delete;

        explicit BasicStreamWriter(T& container);
        ~BasicStreamWriter() = default;

        template <std::integral U>
        void Write(U value);

        void WriteString(const std::string& str);
        void WriteBuffer(std::span<const char> buffer);

    protected:
        std::optional<std::back_insert_iterator<T>> _iter = std::nullopt;
    };

    template <stream_writable_concept T>
    BasicStreamWriter<T>::BasicStreamWriter(T& container)
        : _iter(std::back_inserter(container))
    {
    }

    template <stream_writable_concept T>
    template <std::integral U>
    void BasicStreamWriter<T>::Write(U value)
    {
        const char* begin = reinterpret_cast<const char*>(&value);
        const char* end = begin + sizeof(value);

        assert(_iter.has_value());
        std::copy(begin, end, *_iter);
    }

    template <stream_writable_concept T>
    void BasicStreamWriter<T>::WriteString(const std::string& str)
    {
        assert(_iter.has_value());
        std::copy(str.begin(), str.end(), *_iter);
    }

    template <stream_writable_concept T>
    void BasicStreamWriter<T>::WriteBuffer(std::span<const char> buffer)
    {
        assert(_iter.has_value());
        std::ranges::copy(buffer, *_iter);
    }
}
