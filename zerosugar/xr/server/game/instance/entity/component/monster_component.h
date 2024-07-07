#pragma once
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"
#include "zerosugar/xr/server/game/instance/entity/component/game_component.h"

namespace zerosugar::xr::data
{
    struct Monster;
}

namespace zerosugar::xr
{
    class MonsterComponent : public GameComponent
    {
    public:
        MonsterComponent() = delete;

        explicit MonsterComponent(const data::Monster& data);
        ~MonsterComponent();

        auto GetData() const -> const data::Monster&;
        auto GetSpawnerId() const->std::optional<game_entity_id_type>;

        void SetSpawner(std::optional<game_entity_id_type> spawner);

    private:
        const data::Monster& _data;

        std::optional<game_entity_id_type> _spawnerId = std::nullopt;
    };
}
