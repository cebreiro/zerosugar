#include "monster_component.h"

namespace zerosugar::xr
{
    MonsterComponent::MonsterComponent(const MonsterData& data)
        : _data(data)
    {
    }

    MonsterComponent::~MonsterComponent()
    {
    }

    auto MonsterComponent::GetSpawnerId() const -> std::optional<game_entity_id_type>
    {
        return _spawnerId;
    }

    void MonsterComponent::SetSpawner(std::optional<game_entity_id_type> spawner)
    {
        _spawnerId = spawner;
    }

    auto MonsterComponent::GetData() const -> const MonsterData&
    {
        return _data;
    }
}
