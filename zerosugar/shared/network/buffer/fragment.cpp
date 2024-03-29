#include "fragment.h"

namespace zerosugar::buffer
{
    Fragment::Fragment(SharedPtrNotNull<char[]> ptr, int64_t startOffset, int64_t size)
        : _ptr(std::move(ptr))
        , _startOffset(startOffset)
        , _size(size)
    {
    }

    bool Fragment::IsValid() const
    {
        return _ptr != nullptr && _size > 0;
    }

    auto Fragment::SliceFront(int64_t size) -> std::optional<Fragment>
    {
        if (GetSize() < size)
        {
            return std::nullopt;
        }

        Fragment temp(_ptr, _startOffset, size);

        _startOffset += size;
        _size -= size;

        return temp;
    }

    auto Fragment::GetData() -> char*
    {
        if (!_ptr)
        {
            return nullptr;
        }

        return _ptr.get() + _startOffset;
    }

    auto Fragment::GetData() const -> const char*
    {
        if (!_ptr)
        {
            return nullptr;
        }

        return _ptr.get() + _startOffset;
    }

    auto Fragment::GetSize() const -> int64_t
    {
        return _size;
    }

    auto Fragment::GetSpan() const -> std::span<const char>
    {
        return std::span{ GetData(), static_cast<size_t>(GetSize()) };
    }

    auto Fragment::CreateFrom(std::span<const char> span) -> Fragment
    {
        const auto size = std::ssize(span);
        auto buffer = std::make_shared<char[]>(size);

        std::ranges::copy(span, buffer.get());

        return { std::move(buffer), 0, size };
    }

    auto Fragment::Create(int64_t size) -> Fragment
    {
        assert(size > 0);

        auto buffer = std::make_shared<char[]>(size);
        return { std::move(buffer), 0, size };
    }
}
