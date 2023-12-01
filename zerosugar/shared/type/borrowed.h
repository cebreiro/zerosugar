#pragma once
#include <cassert>
#include <concepts>
#include <zerosugar/shared/type/not_null_pointer.h>

namespace zerosugar
{
    template <typename T>
    concept borrowed_value_concept = requires (T t)
    {
        requires std::move_constructible<T>;
    };

    template <typename T, borrowed_value_concept TBorrowed>
    concept value_owner_concept = requires (T t, TBorrowed& b)
    {
        { t.TakeBack(b) };
    };

    template <borrowed_value_concept T, typename U>
        requires value_owner_concept<U, T>
    class Borrowed
    {
    public:
        Borrowed(const Borrowed& other) = delete;
        Borrowed& operator=(const Borrowed& other) = delete;

        Borrowed() = default;
        Borrowed(Borrowed&& other) noexcept;
        Borrowed& operator=(Borrowed&& other) noexcept;

        Borrowed(T& item, SharedPtrNotNull<U> owner);
        ~Borrowed();

        bool IsValid() const;

        auto operator*() -> T&;
        auto operator*() const -> const T&;
        auto operator->() -> T*;
        auto operator->() const -> const T*;

    private:
        T* _item = nullptr;
        SharedPtrNotNull<U> _owner; 
    };

    template <borrowed_value_concept T, typename U> requires value_owner_concept<U, T>
    Borrowed<T, U>::Borrowed(Borrowed&& other) noexcept
        : _item(std::exchange(other._item, nullptr))
        , _owner(std::move(other._owner))
    {
    }

    template <borrowed_value_concept T, typename U> requires value_owner_concept<U, T>
    Borrowed<T, U>& Borrowed<T, U>::operator=(Borrowed&& other) noexcept
    {
        if (IsValid())
        {
            this->~Borrowed();
        }

        _item = std::exchange(other._item, nullptr);
        _owner = std::move(other._owner);

        return *this;
    }

    template <borrowed_value_concept T, typename U> requires value_owner_concept<U, T>
    Borrowed<T, U>::Borrowed(T& item, SharedPtrNotNull<U> owner)
        : _item(&item)
        , _owner(std::move(owner))
    {
        assert(_item != nullptr);
        assert(_owner.operator bool());
    }

    template <borrowed_value_concept T, typename U> requires value_owner_concept<U, T>
    Borrowed<T, U>::~Borrowed()
    {
        if (!_item)
        {
            return;
        }

        _owner->TakeBack(*_item);
    }

    template <borrowed_value_concept T, typename U> requires value_owner_concept<U, T>
    bool Borrowed<T, U>::IsValid() const
    {
        return _item;
    }

    template <borrowed_value_concept T, typename U> requires value_owner_concept<U, T>
    auto Borrowed<T, U>::operator*() -> T&
    {
        assert(IsValid());
        return *_item;
    }

    template <borrowed_value_concept T, typename U> requires value_owner_concept<U, T>
    auto Borrowed<T, U>::operator*() const -> const T&
    {
        assert(IsValid());
        return *_item;
    }

    template <borrowed_value_concept T, typename U> requires value_owner_concept<U, T>
    auto Borrowed<T, U>::operator->() -> T*
    {
        assert(IsValid());
        return _item;
    }

    template <borrowed_value_concept T, typename U> requires value_owner_concept<U, T>
    auto Borrowed<T, U>::operator->() const -> const T*
    {
        assert(IsValid());
        return _item;
    }
}