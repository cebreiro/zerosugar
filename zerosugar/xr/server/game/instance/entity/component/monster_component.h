#pragma once
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"
#include "zerosugar/xr/server/game/instance/entity/component/game_component.h"

namespace zerosugar::xr
{
    class MonsterData;
}

namespace zerosugar::xr
{
    class MonsterComponent : public GameComponent
    {
    public:
        MonsterComponent() = delete;

        explicit MonsterComponent(const MonsterData& data);
        ~MonsterComponent();

        auto GetData() const -> const MonsterData&;
        auto GetSpawnerId() const->std::optional<game_entity_id_type>;

        void SetSpawner(std::optional<game_entity_id_type> spawner);

    private:
        const MonsterData& _data;

        std::optional<game_entity_id_type> _spawnerId = std::nullopt;
    };
}
