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
