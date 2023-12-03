#include "buffer.h"

#include <cassert>
#include <ios>
#include <numeric>
#include <sstream>

namespace zerosugar
{
    Buffer::Buffer(Buffer&& other) noexcept
        : _size(std::exchange(other._size, 0))
        , _fragments(std::move(other._fragments))
    {
    }

    Buffer& Buffer::operator=(Buffer&& other) noexcept
    {
        _size = std::exchange(other._size, 0);
        _fragments = std::move(other._fragments);

        return *this;
    }

    void Buffer::Add(buffer::Fragment fragment)
    {
        if (!fragment.IsValid())
        {
            return;
        }

        _size += fragment.GetSize();
        _fragments.emplace_back(std::move(fragment));

        assert(GetSize() == CalculateSize());
    }

    void Buffer::Splice(fragment_container_type& container,
        fragment_container_type::iterator begin, fragment_container_type::iterator end)
    {
        for (const buffer::Fragment& fragment : std::ranges::subrange(begin, end))
        {
            _size += fragment.GetSize();
        }

        _fragments.splice(_fragments.end(), container, begin, end);

        assert(GetSize() == CalculateSize());
    }

    bool Buffer::SliceFront(Buffer& result, int64_t size)
    {
        if (GetSize() < size)
        {
            return false;
        }

        Buffer newBuffer;
        int64_t sum = 0;

        for (auto iter = _fragments.begin(); iter != _fragments.end(); ++iter)
        {
            buffer::Fragment& fragment = *iter;
            const int64_t newSum = sum + fragment.GetSize();

            if (newSum > size)
            {
                newBuffer._size = size;
                newBuffer._fragments.splice(newBuffer._fragments.begin(),
                    _fragments, _fragments.begin(), iter);

                std::optional<buffer::Fragment> newFragment = iter->SliceFront(size - sum);
                assert(newFragment.has_value());
                assert(fragment.IsValid());

                newBuffer._fragments.emplace_back(std::move(newFragment.value()));

                break;
            }
            else if (newSum == size)
            {
                newBuffer._size = size;
                newBuffer._fragments.splice(newBuffer._fragments.begin(),
                    _fragments, _fragments.begin(), ++iter);

                break;
            }

            sum = newSum;
        }

        _size -= size;

        assert(GetSize() == CalculateSize());
        assert(newBuffer.GetSize() == newBuffer.CalculateSize());

        result = std::move(newBuffer);

        return true;
    }

    void Buffer::MergeBack(Buffer buffer)
    {
        _size += buffer.GetSize();
        _fragments.splice(_fragments.end(), std::move(buffer._fragments));

        buffer.Clear();

        assert(GetSize() == CalculateSize());
        assert(buffer.GetSize() == buffer.CalculateSize());
    }

    void Buffer::MergeBack(fragment_container_type fragments)
    {
        _size += CalculateSize(fragments);
        _fragments.splice(_fragments.end(), std::move(fragments));

        assert(GetSize() == CalculateSize());
    }

    auto Buffer::Clone() -> Buffer
    {
        if (_size == 0)
        {
            return {};
        }

        Buffer result;
        result.Add(buffer::Fragment([this]() -> std::shared_ptr<char[]>
            {
                auto buffer = std::make_shared<char[]>(_size);
                auto range = _fragments | std::views::transform([](const buffer::Fragment& fragment) -> std::span<const char>
                    {
                        return fragment.GetSpan();
                    }) | std::views::join;
                std::ranges::copy(range, buffer.get());

                return buffer;
            }(), 0, _size));
        return result;
    }

    void Buffer::Clear()
    {
        _size = 0;
        _fragments.clear();
    }

    auto Buffer::GetSize() const -> int64_t
    {
        return _size;
    }

    auto Buffer::GetFragmentContainer() -> fragment_container_type&
    {
        return _fragments;
    }

    auto Buffer::GetFragmentContainer() const -> const fragment_container_type&
    {
        return _fragments;
    }

    auto Buffer::begin() -> iterator
    {
        return iterator(_fragments.begin(), _fragments.end(), 0);
    }

    auto Buffer::end() -> iterator
    {
        return iterator(_fragments.end(), _fragments.end(), 0);
    }

    auto Buffer::begin() const -> const_iterator
    {
        return const_iterator(_fragments.begin(), _fragments.end(), 0);
    }

    auto Buffer::end() const -> const_iterator
    {
        return const_iterator(_fragments.end(), _fragments.end(), 0);
    }

    auto Buffer::cbegin() const -> const_iterator
    {
        return begin();
    }

    auto Buffer::cend() const -> const_iterator
    {
        return end();
    }

    auto Buffer::ToString() const -> std::string
    {
        std::ostringstream oss;
        oss << std::uppercase;
        oss << std::hex;

        int32_t temp = 0;
        for (char c : *this)
        {
            temp = c & 0xFF;
            if (temp >= 0 && temp <= 0xF)
            {
                oss << '0';
            }

            oss << temp;
            oss << " ";
        }

        std::string str = oss.str();
        if (!str.empty())
        {
            str.pop_back();
        }

        return str;
    }

    auto Buffer::CalculateSize() const -> int64_t
    {
        return CalculateSize(_fragments);
    }

    auto Buffer::CalculateSize(const fragment_container_type& fragments) -> int64_t
    {
        return std::accumulate(fragments.begin(), fragments.end(), int64_t{ 0 },
            [](int64_t sum, const buffer::Fragment& fragment) -> int64_t
            {
                return sum + fragment.GetSize();
            });
    }

    Buffer::iterator::iterator(fragment_container_type::iterator iter,
        fragment_container_type::iterator end, int64_t index)
        : _iter(iter)
        , _end(end)
        , _index(index)
    {
        if (_iter != _end && _index >= _iter->GetSize())
        {
            ++(*this);
        }
    }

    auto Buffer::iterator::operator*() const -> reference
    {
        buffer::Fragment& fragment = *_iter;

        assert(_index >= 0 && _index <= fragment.GetSize());

        return *(fragment.GetData() + _index);
    }

    auto Buffer::iterator::operator->() const -> pointer
    {
        buffer::Fragment& fragment = *_iter;

        assert(_index >= 0 && _index <= fragment.GetSize());

        return fragment.GetData() + _index;
    }

    auto Buffer::iterator::operator++() -> iterator&
    {
        auto iter = _iter;
        int64_t index = _index;

        if (++index >= iter->GetSize())
        {
            while (true)
            {
                ++iter;
                index = 0;

                if (iter == _end || iter->GetSize() > 0)
                {
                    break;
                }
            }
        }

        _iter = iter;
        _index = index;

        return *this;
    }

    auto Buffer::iterator::operator++(int) -> iterator
    {
        iterator temp(*this);
        ++(*this);

        return temp;
    }

    bool Buffer::iterator::operator==(const iterator& other) const
    {
        return _iter == other._iter && _end == other._end && _index == other._index;
    }

    bool Buffer::iterator::operator!=(const iterator& other) const
    {
        return _iter != other._iter || _end != other._end || _index != other._index;
    }

    Buffer::const_iterator::const_iterator(fragment_container_type::const_iterator iter,
        fragment_container_type::const_iterator end, int64_t index)
        : _iter(iter)
        , _end(end)
        , _index(index)
    {
        if (_iter != _end && _index >= _iter->GetSize())
        {
            ++(*this);
        }
    }

    auto Buffer::const_iterator::operator*() const -> reference
    {
        const buffer::Fragment& fragment = *_iter;

        assert(_index >= 0 && _index <= fragment.GetSize());

        return *(fragment.GetData() + _index);
    }

    auto Buffer::const_iterator::operator->() const -> pointer
    {
        const buffer::Fragment& fragment = *_iter;

        assert(_index >= 0 && _index <= fragment.GetSize());

        return fragment.GetData() + _index;
    }

    auto Buffer::const_iterator::operator++() -> const_iterator&
    {
        auto iter = _iter;
        int64_t index = _index;

        if (++index >= iter->GetSize())
        {
            while (true)
            {
                ++iter;
                index = 0;

                if (iter == _end || iter->GetSize() > 0)
                {
                    break;
                }
            }
        }

        _iter = iter;
        _index = index;

        return *this;
    }

    auto Buffer::const_iterator::operator++(int) -> const_iterator
    {
        const_iterator temp(*this);
        ++(*this);

        return temp;
    }

    bool Buffer::const_iterator::operator==(const const_iterator& other) const
    {
        return _iter == other._iter && _end == other._end && _index == other._index;
    }

    bool Buffer::const_iterator::operator!=(const const_iterator& other) const
    {
        return _iter != other._iter || _end != other._end || _index != other._index;
    }
}
