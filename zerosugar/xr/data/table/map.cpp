#include "map.h"

namespace zerosugar::xr::data
{
    void from_json(const nlohmann::json& json, Portal& portal)
    {
        json.at("Mapid").get_to(portal.linkedMapId);
        json.at("x").get_to(portal.x);
        json.at("y").get_to(portal.y);
        json.at("z").get_to(portal.z);
    }

    void from_json(const nlohmann::json& json, PlayerSpawnPoint& spawnPoint)
    {
        json.at("x").get_to(spawnPoint.x);
        json.at("y").get_to(spawnPoint.y);
        json.at("z").get_to(spawnPoint.z);
        json.at("rd").get_to(spawnPoint.yaw);
    }

    void from_json(const nlohmann::json& json, MonsterSpawner& spawner)
    {
        json.at("id").get_to(spawner.monsterId);
        json.at("x").get_to(spawner.x);
        json.at("y").get_to(spawner.y);
        json.at("z").get_to(spawner.z);
    }

    void from_json(const nlohmann::json& json, Map& map)
    {
        json.at("Mapid").get_to(map.id);

        int32_t type = 0;
        json.at("Type").get_to(type);

        map.type = static_cast<MapType>(type);
        assert(IsValid(map.type));

        json.at("PlayerSpawn").get_to(map.playerSpawnPoint);

        if (auto iter = json.find("SpawnPoint"); iter != json.end())
        {
            iter->get_to(map.monsterSpawners);
        }
    }
}

namespace zerosugar::xr
{
    MapData::MapData(data::Map map)
        : _map(std::move(map))
    {
    }

    auto MapData::GetId() const -> int32_t
    {
        return _map.id;
    }

    auto MapData::GetType() const -> data::MapType
    {
        return _map.type;
    }

    auto MapData::GetPortal() const -> const data::Portal*
    {
        return nullptr;
    }

    auto MapData::GetPlayerSpawnPoint() const -> const data::PlayerSpawnPoint&
    {
        return _map.playerSpawnPoint;
    }

    auto MapData::GetMonsterSpawners() const -> std::span<const data::MonsterSpawner>
    {
        return _map.monsterSpawners;
    }
}
