#include "server_config.h"

namespace zerosugar::sl
{
    void from_json(const nlohmann::json& j, ZoneServerConfig& config)
    {
        config._zoneId = j.at("zone_id").get<int32_t>();
        
        config._port = j.at("port").get<uint16_t>();
    }

    auto ZoneServerConfig::GetZoneId() const -> int32_t
    {
        return _zoneId;
    }

    auto ZoneServerConfig::GetPort() const -> uint16_t
    {
        return _port;
    }

    void from_json(const nlohmann::json& j, WorldConfig& config)
    {
        config._worldId = j.at("world_id");
        config._zoneServerConfigs = j.at("zone").get<std::vector<ZoneServerConfig>>();
    }

    auto WorldConfig::GetWorldId() const -> int8_t
    {
        return _worldId;
    }

    auto WorldConfig::GetZoneServerConfigs() const -> const std::vector<ZoneServerConfig>&
    {
        return _zoneServerConfigs;
    }

    void from_json(const nlohmann::json& j, ServerConfig& config)
    {
        config._address = j.at("address").get<std::string>();
        config._worldConfigs = j.at("world").get<std::vector<WorldConfig>>();
    }

    auto ServerConfig::GetAddress() const -> const std::string&
    {
        return _address;
    }

    auto ServerConfig::GetWorldConfigs() const -> const std::vector<WorldConfig>&
    {
        return _worldConfigs;
    }
}
