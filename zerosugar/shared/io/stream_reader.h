#pragma once
#include <cstdint>
#include <cassert>
#include <concepts>
#include <string>
#include <iterator>
#include <stdexcept>

namespace zerosugar
{
    template <typename T>
    concept stream_readable_concept = requires (T t)
    {
        { ++t } -> std::same_as<T&>;
        { t++ } -> std::same_as<T>;
        requires std::convertible_to<typename T::value_type, const char>;
    };

    template <stream_readable_concept T>
    class StreamReader
    {
    public:
        using value_type = std::decay_t<T>;

    public:
        StreamReader() = delete;
        StreamReader(const StreamReader&) = delete;
        StreamReader(StreamReader&&) noexcept = delete;
        StreamReader& operator=(const StreamReader&) = delete;
        StreamReader& operator=(StreamReader&&) noexcept = delete;

        StreamReader(T begin, T end);
        ~StreamReader() = default;

        bool Empty() const noexcept;
        bool CanRead(int64_t size) const noexcept;

        template <std::integral U>
        auto Read() -> U;

        template <std::floating_point U>
        auto Read() -> U;

        auto ReadString() -> std::string;
        auto ReadString(int64_t size) -> std::string;
        void ReadBuffer(char* buffer, int64_t size);

        auto begin() const -> T;
        auto end() const -> T;

        auto GetReadSize() const -> int64_t;

    private:
        T _begin;
        T _iter;
        T _end;
        int64_t _readSize = 0;
        int64_t _remainSize = 0;
    };

    template <stream_readable_concept T>
    StreamReader<T>::StreamReader(T begin, T end)
        : _begin(begin)
        , _iter(std::move(begin))
        , _end(std::move(end))
        , _remainSize(std::distance(_iter, _end))
    {
    }

    template <stream_readable_concept T>
    bool StreamReader<T>::Empty() const noexcept
    {
        assert(_remainSize == std::distance(_iter, _end));

        return _remainSize == 0;
    }

    template <stream_readable_concept T>
    bool StreamReader<T>::CanRead(int64_t size) const noexcept
    {
        assert(_remainSize == std::distance(_iter, _end));

        return _remainSize >= size;
    }

    template <stream_readable_concept T>
    template <std::integral U>
    auto StreamReader<T>::Read() -> U
    {
        constexpr int64_t size = sizeof(U);

        if (!CanRead(size))
        {
            throw std::runtime_error("fail to reader stream");
        }

        std::remove_const_t<U> result = 0;

        for (int64_t i = 0; i < size; ++i)
        {
            result += (static_cast<U>(*_iter) & 0xFF) << i * 8;
            ++_iter;
        }

        _readSize += size;
        _remainSize -= size;
        assert(_remainSize == std::distance(_iter, _end));

        return result;
    }

    template <stream_readable_concept T>
    template <std::floating_point U>
    auto StreamReader<T>::Read() -> U
    {
        constexpr int64_t size = sizeof(U);

        if (!CanRead(size))
        {
            throw std::runtime_error("fail to reader stream");
        }

        auto iter = _iter;
        std::advance(_iter, size);

        std::array<char, size> buffer;
        std::copy(iter, _iter, buffer.begin());

        _readSize += size;
        _remainSize -= size;
        assert(_remainSize == std::distance(_iter, _end));

        return *reinterpret_cast<U*>(buffer.data());
    }

    template <stream_readable_concept T>
    auto StreamReader<T>::ReadString() -> std::string
    {
        int64_t size = 0;
        auto iter = _iter;

        while (iter != _end)
        {
            if (*iter == '\0')
            {
                return ReadString(size);
            }

            ++size;
            ++iter;
        }

        return {};
    }

    template <stream_readable_concept T>
    auto StreamReader<T>::ReadString(int64_t size) -> std::string
    {
        if (size <= 0)
        {
            return {};
        }

        if (!CanRead(size))
        {
            throw std::runtime_error("fail to reader stream");
        }

        std::string result(size + 1, 0);

        for (int64_t i = 0; i < size; ++i)
        {
            result[i] = *_iter;
            ++_iter;
        }

        _readSize += size;
        _remainSize -= size;
        assert(_remainSize == std::distance(_iter, _end));

        return result;
    }

    template <stream_readable_concept T>
    void StreamReader<T>::ReadBuffer(char* buffer, int64_t size)
    {
        if (!CanRead(size))
        {
            assert(false);
            return;
        }

        for (int64_t i = 0; i < size; ++i)
        {
            buffer[i] = *_iter;
            ++_iter;
        }

        _readSize += size;
        _remainSize -= size;
        assert(_remainSize == std::distance(_iter, _end));
    }

    template <stream_readable_concept T>
    auto StreamReader<T>::begin() const -> T
    {
        return _begin;
    }

    template <stream_readable_concept T>
    auto StreamReader<T>::end() const -> T
    {
        return _end;
    }

    template <stream_readable_concept T>
    auto StreamReader<T>::GetReadSize() const -> int64_t
    {
        return _readSize;
    }
}
