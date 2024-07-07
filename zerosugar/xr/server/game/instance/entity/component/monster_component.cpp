#include "monster_component.h"

namespace zerosugar::xr
{
    MonsterComponent::MonsterComponent(const data::Monster& data)
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

    auto MonsterComponent::GetData() const -> const data::Monster&
    {
        return _data;
    }
}
