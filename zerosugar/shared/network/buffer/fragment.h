#pragma once
#include <cstdint>
#include <span>
#include <memory>
#include <optional>
#include <span>
#include "zerosugar/shared/type/not_null_pointer.h"

namespace zerosugar::buffer
{
    class Fragment
    {
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

        static auto CreateFrom(std::span<const char> span) -> Fragment;
        static auto Create(int64_t size) -> Fragment;

    private:
        SharedPtrNotNull<char[]> _ptr;
        int64_t _startOffset = 0;
        int64_t _size = 0;
    };
}

