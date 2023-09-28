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
    class ServiceLocatorT
    {
    public:
        /*explicit*/ ServiceLocatorT(ServiceLocator& serviceLocator)
            : _tuple({ serviceLocator.Find<TServices>()... })
        {
        }

        template <typename U> requires std::disjunction_v<std::is_same<U, TServices>...>
        auto Find() -> U*
        {
            return std::get<U*>(_tuple);
        }

        template <typename U> requires std::disjunction_v<std::is_same<U, TServices>...>
        auto Find() const -> const U*
        {
            return std::get<U*>(_tuple);
        }

    private:
        std::tuple<TServices*...> _tuple;
    };
}
