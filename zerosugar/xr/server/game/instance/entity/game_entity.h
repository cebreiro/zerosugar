#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"
#include "zerosugar/xr/server/game/instance/entity/component/game_component.h"

namespace zerosugar::xr
{
    class IGameController;
}

namespace zerosugar::xr
{
    class GameEntity
    {
    public:
        GameEntity() = default;

        template <typename T> requires std::derived_from<T, GameComponent>
        bool AddComponent(UniquePtrNotNull<T> component);

        template <typename T> requires std::derived_from<T, GameComponent>
        bool RemoveComponent();

        template <typename T> requires std::derived_from<T, GameComponent>
        auto FindComponent() -> T*;

        template <typename T> requires std::derived_from<T, GameComponent>
        auto FindComponent() const -> const T*;

        template <typename T> requires std::derived_from<T, GameComponent>
        bool FindAndThen(const std::function<void(T&)>& func);

        template <typename T> requires std::derived_from<T, GameComponent>
        bool FindAndThen(const std::function<void(const T&)>& func) const;

        template <typename T> requires std::derived_from<T, GameComponent>
        auto GetComponent() -> T&;

        template <typename T> requires std::derived_from<T, GameComponent>
        auto GetComponent() const -> const T&;


        auto GetId() const -> game_entity_id_type;
        auto GetController() const -> IGameController&;

        void SetId(game_entity_id_type id);
        void SetController(SharedPtrNotNull<IGameController> controller);

    private:
        game_entity_id_type _id;
        SharedPtrNotNull<IGameController> _controller;
        boost::unordered::unordered_flat_map<game_component_id_type, UniquePtrNotNull<GameComponent>> _components;
    };

    template <typename T> requires std::derived_from<T, GameComponent>
    bool GameEntity::AddComponent(UniquePtrNotNull<T> component)
    {
        assert(component);

        const auto& id = GameComponent::GetClassId<T>();

        const auto& [iter, inserted] = _components.try_emplace(id, nullptr);
        if (inserted)
        {
            iter->second = std::move(component);
        }

        return inserted;
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    bool GameEntity::RemoveComponent()
    {
        const auto& id = GameComponent::GetClassId<T>();

        return _components.erase(id);
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    auto GameEntity::FindComponent() -> T*
    {
        const auto& id = GameComponent::GetClassId<T>();

        const auto iter = _components.find(id);
        if (iter != _components.end())
        {
            assert(iter->second);

            T* result = static_cast<T*>(iter->second.get());
            assert(result == dynamic_cast<T*>(iter->second.get()));

            return result;
        }

        return nullptr;
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    auto GameEntity::FindComponent() const -> const T*
    {
        const auto& id = GameComponent::GetClassId<T>();

        const auto iter = _components.find(id);
        if (iter != _components.end())
        {
            assert(iter->second);

            const T* result = static_cast<const T*>(iter->second.get());
            assert(result == dynamic_cast<const T*>(iter->second.get()));

            return result;
        }

        return nullptr;
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    bool GameEntity::FindAndThen(const std::function<void(T&)>& func)
    {
        assert(func);

        if (T* component = FindComponent<T>(); component)
        {
            func(*component);

            return true;
        }

        return false;
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    bool GameEntity::FindAndThen(const std::function<void(const T&)>& func) const
    {
        assert(func);

        if (const T* component = FindComponent<T>(); component)
        {
            func(*component);

            return true;
        }

        return false;
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    auto GameEntity::GetComponent() -> T&
    {
        T* component = FindComponent<T>();
        assert(component);

        return *component;
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    auto GameEntity::GetComponent() const -> const T&
    {
        const T* component = FindComponent<T>();
        assert(component);

        return *component;
    }
}
