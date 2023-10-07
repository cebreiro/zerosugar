#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include "zerosugar/shared/type/not_null_pointer.h"

namespace zerosugar::network::buffer
{
    class Fragment
    {
    public:
        static constexpr int64_t MAX_SIZE = 256;

    public:
        Fragment(const Fragment& other) = delete;
        Fragment& operator=(const Fragment& other) = delete;

        Fragment() = default;
        ~Fragment() = default;
        Fragment(Fragment&& other) noexcept = default;
        Fragment& operator=(Fragment&& other) noexcept = default;

        Fragment(SharedPtrNotNull<char[]> ptr, int64_t startOffset, int64_t size);

        bool IsValid() const;

        auto SliceFront(int64_t size) -> std::optional<Fragment>;

        auto GetData() -> char*;
        auto GetData() const -> const char*;
        auto GetSize() const -> int64_t;

        auto GetSpan() const->std::span<const char>;

    private:
        SharedPtrNotNull<char[]> _ptr;
        int64_t _startOffset = 0;
        int64_t _size = 0;
    };
}

