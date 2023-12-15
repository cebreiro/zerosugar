#pragma once

namespace zerosugar::sl
{
    class ZoneServerConfig
    {
    private:
        friend void from_json(const nlohmann::json& j, ZoneServerConfig& config);

    public:
        ZoneServerConfig() = default;

        auto GetZoneId() const -> int32_t;
        auto GetPort() const -> uint16_t;

    private:
        int32_t _zoneId = 0;
        uint16_t _port = 0;
    };

    class WorldConfig
    {
    private:
        friend void from_json(const nlohmann::json& j, WorldConfig& config);

    public:
        auto GetWorldId() const -> int8_t;
        auto GetZoneServerConfigs() const -> const std::vector<ZoneServerConfig>&;

    private:
        int8_t _worldId = 0;
        std::vector<ZoneServerConfig> _zoneServerConfigs;
    };

    class ServerConfig
    {
    private:
        friend void from_json(const nlohmann::json& j, ServerConfig& config);

    public:
        ServerConfig() = default;

        auto GetAddress() const -> const std::string&;
        auto GetWorldConfigs() const -> const std::vector<WorldConfig>&;

    private:
        std::string _address;
        std::vector<WorldConfig> _worldConfigs;
    };
}
