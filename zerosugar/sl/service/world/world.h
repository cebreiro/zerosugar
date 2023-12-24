#pragma once
#include "tbb/concurrent_unordered_map.h"

namespace zerosugar::sl
{
    class Zone;

    class World
    {
    public:
        World() = delete;

        World(execution::IExecutor& executor, int8_t id, std::string address);
        ~World();

        bool CreateZone(int32_t zoneId, std::string address, uint16_t port);

        auto GetId() const -> int8_t;
        auto GetAddress() const -> const std::string&;

    private:
        int8_t _id = 0;
        std::string _address;

        tbb::concurrent_unordered_map<int32_t, UniquePtrNotNull<Zone>> _zones;
    };
}
