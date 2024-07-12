#include "visual_object_container.h"

namespace zerosugar::xr::bot
{
    bool VisualObjectContainer::Add(int64_t id, SharedPtrNotNull<Monster> monster)
    {
        return _monsters.try_emplace(id, std::move(monster)).second;
    }

    bool VisualObjectContainer::Remove(int64_t id)
    {
        return _monsters.erase(id);
    }

    void VisualObjectContainer::Clear()
    {
        _monsters.clear();
    }

    bool VisualObjectContainer::HasMonster() const
    {
        return !_monsters.empty();
    }

    bool VisualObjectContainer::Contains(int64_t id) const
    {
        return FindMonster(id) != nullptr;
    }

    auto VisualObjectContainer::FindMonster(int64_t id) -> Monster*
    {
        const auto iter = _monsters.find(id);

        return iter != _monsters.end() ? iter->second.get() : nullptr;
    }

    auto VisualObjectContainer::FindMonster(int64_t id) const -> const Monster*
    {
        const auto iter = _monsters.find(id);

        return iter != _monsters.end() ? iter->second.get() : nullptr;
    }
}
