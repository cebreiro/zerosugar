#pragma once
#include <boost/unordered/unordered_flat_map.hpp>

namespace zerosugar::xr::bot
{
    class Monster;
}

namespace zerosugar::xr::bot
{
    class VisualObjectContainer
    {
    public:
        bool Add(int64_t id, SharedPtrNotNull<Monster> monster);
        bool Remove(int64_t id);

        void Clear();

        bool HasMonster() const;
        bool Contains(int64_t id) const;

        auto FindMonster(int64_t id) -> Monster*;
        auto FindMonster(int64_t id) const -> const Monster*;

        inline auto GetMonsterRange();
        inline auto GetMonsterRange() const;

    private:
        boost::unordered::unordered_flat_map<int64_t, SharedPtrNotNull<Monster>> _monsters;
    };

    auto VisualObjectContainer::GetMonsterRange()
    {
        return _monsters
            | std::views::values
            | std::views::transform([](const SharedPtrNotNull<Monster>& monster) -> Monster&
                {
                    return *monster;
                });
    }

    auto VisualObjectContainer::GetMonsterRange() const
    {
        return _monsters
            | std::views::values
            | std::views::transform([](const SharedPtrNotNull<Monster>& monster) -> const Monster&
                {
                    return *monster;
                });
    }
}
