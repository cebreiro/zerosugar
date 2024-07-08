#pragma once
#include "zerosugar/xr/data/enum/map_type.h"

namespace zerosugar::xr::data
{
    struct Portal
    {
        int32_t linkedMapId = 0;
        float x = 0.f;
        float y = 0.f;
        float z = 0.f;

        friend void from_json(const nlohmann::json& json, Portal& portal);
    };

    struct PlayerSpawnPoint
    {
        float x = 0.f;
        float y = 0.f;
        float z = 0.f;
        float yaw = 0.f;

        friend void from_json(const nlohmann::json& json, PlayerSpawnPoint& spawnPoint);
    };

    struct MonsterSpawner
    {
        int32_t monsterId = 0;
        float x = 0.f;
        float y = 0.f;
        float z = 0.f;

        friend void from_json(const nlohmann::json& json, MonsterSpawner& spawner);
    };

    struct Map
    {
        int32_t id = 0;
        MapType type = MapType::Village;
        PlayerSpawnPoint playerSpawnPoint = {};
        std::vector<MonsterSpawner> monsterSpawners;

        friend void from_json(const nlohmann::json& json, Map& map);
    };
}

namespace zerosugar::xr
{
    class MapData
    {
    public:
        MapData() = default;
        explicit MapData(data::Map map);

        auto GetId() const -> int32_t;
        auto GetType() const -> data::MapType;
        auto GetPortal() const -> const data::Portal*;
        auto GetPlayerSpawnPoint() const -> const data::PlayerSpawnPoint&;
        auto GetMonsterSpawners() const -> std::span<const data::MonsterSpawner>;

    private:
        data::Map _map;
    };
}
