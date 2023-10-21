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
        requires std::same_as<typename T::value_type, const char>;
    };

    template <stream_readable_concept T>
    class BasicStreamReader
    {
    public:
        using value_type = T;

    public:
        BasicStreamReader() = delete;
        BasicStreamReader(const BasicStreamReader&) = delete;
        BasicStreamReader(BasicStreamReader&&) noexcept = delete;
        BasicStreamReader& operator=(const BasicStreamReader&) = delete;
        BasicStreamReader& operator=(BasicStreamReader&&) noexcept = delete;

        BasicStreamReader(const T& begin, const T& end);
        ~BasicStreamReader() = default;

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

    private:
        T _begin;
        T _iter;
        T _end;
        int64_t _remainSize = 0;
    };

    template <stream_readable_concept T>
    BasicStreamReader<T>::BasicStreamReader(const T& begin, const T& end)
        : _begin(begin)
        , _iter(begin)
        , _end(end)
        , _remainSize(std::distance(_iter, _end))
    {
    }

    template <stream_readable_concept T>
    bool BasicStreamReader<T>::Empty() const noexcept
    {
        assert(_remainSize == std::distance(_iter, _end));

        return _remainSize == 0;
    }

    template <stream_readable_concept T>
    bool BasicStreamReader<T>::CanRead(int64_t size) const noexcept
    {
        assert(_remainSize == std::distance(_iter, _end));

        return _remainSize >= size;
    }

    template <stream_readable_concept T>
    template <std::integral U>
    auto BasicStreamReader<T>::Read() -> U
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

        _remainSize -= size;
        assert(_remainSize == std::distance(_iter, _end));

        return result;
    }

    template <stream_readable_concept T>
    template <std::floating_point U>
    auto BasicStreamReader<T>::Read() -> U
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

        _remainSize -= size;
        assert(_remainSize == std::distance(_iter, _end));

        return *reinterpret_cast<U*>(buffer.data());
    }

    template <stream_readable_concept T>
    auto BasicStreamReader<T>::ReadString() -> std::string
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
    auto BasicStreamReader<T>::ReadString(int64_t size) -> std::string
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

        _remainSize -= size;
        assert(_remainSize == std::distance(_iter, _end));

        return result;
    }

    template <stream_readable_concept T>
    void BasicStreamReader<T>::ReadBuffer(char* buffer, int64_t size)
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

        _remainSize -= size;
        assert(_remainSize == std::distance(_iter, _end));
    }

    template <stream_readable_concept T>
    auto BasicStreamReader<T>::begin() const -> T
    {
        return _begin;
    }

    template <stream_readable_concept T>
    auto BasicStreamReader<T>::end() const -> T
    {
        return _end;
    }
}
