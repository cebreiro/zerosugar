#pragma once
#include <memory>
#include <boost/container/flat_map.hpp>
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/type/not_null_pointer.h"
#include "zerosugar/shared/type/runtime_type_id.h"

namespace zerosugar
{
    class ServiceLocator
    {
        using service_id_type = RuntimeTypeId<IService>;

    public:
        ServiceLocator(const ServiceLocator&) = default;
        ServiceLocator& operator=(const ServiceLocator&) = default;
        ServiceLocator() = default;

        template <std::derived_from<IService> T>
        bool Add(SharedPtrNotNull<IService> service);

        template <std::derived_from<IService> T>
        bool Remove();

        template <std::derived_from<IService> T>
        auto Find() -> T*;

        template <std::derived_from<IService> T>
        auto Find() const -> const T*;

    private:
        boost::container::flat_map<int64_t, SharedPtrNotNull<IService>> _services;
    };

    template <std::derived_from<IService> T>
    bool ServiceLocator::Add(SharedPtrNotNull<IService> service)
    {
        const int64_t id = service_id_type::Get<T>();

        return _services.try_emplace(id, std::move(service)).second;
    }

    template <std::derived_from<IService> T>
    bool ServiceLocator::Remove()
    {
        const int64_t id = service_id_type::Get<T>();

        return _services.erase(id);
    }

    template <std::derived_from<IService> T>
    auto ServiceLocator::Find() -> T*
    {
        const int64_t id = service_id_type::Get<T>();
        auto iter = _services.find(id);

        return iter != _services.end() ? static_cast<T*>(iter->second.get()) : nullptr;
    }

    template <std::derived_from<IService> T>
    auto ServiceLocator::Find() const -> const T*
    {
        const int64_t id = service_id_type::Get<T>();
        auto iter = _services.find(id);

        return iter != _services.end() ? static_cast<const T*>(iter->second.get()) : nullptr;
    }

    template <typename... TServices> requires std::conjunction_v<std::is_base_of<IService, TServices>...>
    class ServiceLocatorRef
    {
    public:
        template <typename T, typename... TArgs>
        using is_one_of = std::disjunction<std::is_same<T, TArgs>...>;

    public:
        ServiceLocatorRef() = default;
        explicit(false) ServiceLocatorRef(ServiceLocator& serviceLocator)
            : _tuple({ serviceLocator.Find<TServices>()... })
        {
        }

        template <typename ...UServices>
            requires std::conjunction_v<is_one_of<TServices, UServices...>...>
        explicit(false) ServiceLocatorRef(ServiceLocatorRef<UServices...>& serviceLocator)
            : _tuple({ serviceLocator.template Find<TServices>()... })
        {
        }

        bool ContainsAll() const
        {
            constexpr auto contains = []<typename T, size_t... I>(const T& tuple, std::index_sequence<I...>)
            {
                return (... & (std::get<I>(tuple) != nullptr));
            };

            return contains(_tuple, std::make_index_sequence<std::tuple_size_v<decltype(_tuple)>>{});
        }

        template <typename U> requires is_one_of<U, TServices...>::value
        bool Contains() const
        {
            return std::get<U*>(_tuple) != nullptr;
        }

        template <typename U> requires is_one_of<U, TServices...>::value
        auto Find() -> U*
        {
            return std::get<U*>(_tuple);
        }

        template <typename U> requires is_one_of<U, TServices...>::value
        auto Find() const -> const U*
        {
            return std::get<U*>(_tuple);
        }

        template <typename U> requires is_one_of<U, TServices...>::value
        auto Get() -> U&
        {
            U* ptr = std::get<U*>(_tuple);
            assert(ptr);

            return *ptr;
        }

        template <typename U> requires is_one_of<U, TServices...>::value
        auto Get() const -> const U&
        {
            const U* ptr = std::get<U*>(_tuple);
            assert(ptr);

            return *ptr;
        }

    private:
        std::tuple<TServices*...> _tuple;
    };
}
