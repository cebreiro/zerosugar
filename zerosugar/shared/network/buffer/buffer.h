#pragma once
#include <list>
#include "zerosugar/shared/network/buffer/fragment.h"

namespace zerosugar
{
    class Buffer
    {
    public:
        using fragment_container_type = std::list<buffer::Fragment>;

    public:
        class iterator;
        class const_iterator;

    public:
        Buffer(const Buffer& other) = delete;
        Buffer& operator=(const Buffer& other) = delete;

        Buffer() = default;
        Buffer(Buffer&& other) noexcept;
        Buffer& operator=(Buffer&& other) noexcept;

        void Add(buffer::Fragment fragment);
        void Splice(fragment_container_type& container,
            fragment_container_type::iterator begin, fragment_container_type::iterator end);

        bool SliceFront(Buffer& result, int64_t size);
        void MergeBack(Buffer buffer);
        void MergeBack(fragment_container_type fragments);

        auto Clone() -> Buffer;

        void Clear();

        auto GetSize() const -> int64_t;
        auto GetFragmentContainer() -> fragment_container_type&;
        auto GetFragmentContainer() const -> const fragment_container_type&;

        auto begin() -> iterator;
        auto end() -> iterator;

        auto begin() const -> const_iterator;
        auto end() const -> const_iterator;

        auto cbegin() const -> const_iterator;
        auto cend() const -> const_iterator;

        auto ToString() const -> std::string;

    private:
        auto CalculateSize() const -> int64_t;
        static auto CalculateSize(const fragment_container_type& fragments) -> int64_t;

    private:
        int64_t _size = 0;
        fragment_container_type _fragments;
    };

    class Buffer::iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = char;
        using pointer = value_type*;
        using reference = value_type&;

    public:
        iterator() = default;
        iterator(const iterator& other) = default;
        iterator& operator=(const iterator& other) = default;

        iterator(fragment_container_type::iterator iter, fragment_container_type::iterator end, int64_t index);

        auto operator*() const -> reference;
        auto operator->() const -> pointer;

        auto operator++() -> iterator&;
        auto operator++(int) -> iterator;

        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;

    private:
        fragment_container_type::iterator _iter;
        fragment_container_type::iterator _end;
        int64_t _index = 0;
    };

    class Buffer::const_iterator : public std::forward_iterator_tag
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const char;
        using pointer = value_type*;
        using reference = value_type&;

    public:
        const_iterator() = default;
        const_iterator(const const_iterator& other) = default;
        const_iterator& operator=(const const_iterator& other) = default;

        const_iterator(fragment_container_type::const_iterator iter, fragment_container_type::const_iterator end, int64_t index);

        auto operator*() const -> reference;
        auto operator->() const -> pointer;

        auto operator++() -> const_iterator&;
        auto operator++(int) -> const_iterator;

        bool operator==(const const_iterator& other) const;
        bool operator!=(const const_iterator& other) const;

    private:
        fragment_container_type::const_iterator _iter;
        fragment_container_type::const_iterator _end;
        int64_t _index = 0;
    };
}
