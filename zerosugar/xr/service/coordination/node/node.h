#pragma once
#include <boost/unordered/unordered_flat_map.hpp>

namespace zerosugar::xr::coordination
{
    namespace detail
    {
        template <typename TParent>
        class NodeParentPart
        {
        public:
            bool HasParent() const
            {
                return _parent != nullptr;
            }

            auto GetParent() -> TParent
            {
                return _parent;
            }

            auto GetParent() const -> const TParent
            {
                return _parent;
            }

            void SetParent(TParent parent)
            {
                assert(!_parent);

                _parent = parent;
            }

        private:
            TParent _parent = nullptr;
        };

        template <>
        class NodeParentPart<void> {};

        template <typename TChild, typename TChildId>
        class NodeChildPart
        {
        public:
            bool AddChild(const TChildId& id, TChild child)
            {
                return _children.try_emplace(id, child).second;
            }

            bool RemoveChild(const TChildId& id)
            {
                return _children.erase(id);
            }

            bool HasChild(const TChildId& id) const
            {
                return _children.contains(id);
            }

            auto FindChild(const TChildId& id) -> TChild
            {
                const auto iter = _children.find(id);

                return iter != _children.end() ? iter->second : nullptr;
            }

            auto FindChild(const TChildId& id) const -> const TChild
            {
                const auto iter = _children.find(id);

                return iter != _children.end() ? iter->second : nullptr;
            }

        private:
            boost::unordered_flat_map<TChildId, TChild> _children;
        };

        template <>
        class NodeChildPart<void, void> {};
    }

    template <typename TParent, typename TChild, typename TChildId>
        requires std::conjunction_v<
            std::disjunction<std::is_pointer<TParent>, std::is_same<TParent, void>>,
            std::disjunction<std::is_pointer<TChild>, std::is_same<TChild, void>>>
    class Node
        : public detail::NodeParentPart<TParent>
        , public detail::NodeChildPart<TChild, TChildId>
    {
    public:
    };
    
}
