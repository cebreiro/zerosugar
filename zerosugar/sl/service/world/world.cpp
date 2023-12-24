#include "world.h"

#include "zerosugar/sl/service/world/zone/zone.h"

namespace zerosugar::sl
{
    World::World(execution::IExecutor& executor, int8_t id, std::string address)
        : _id(id)
        , _address(std::move(address))
    {
        (void)executor;
    }

    World::~World()
    {
    }

    bool World::CreateZone(int32_t zoneId, std::string address, uint16_t port)
    {
        (void)zoneId;
        (void)address;
        (void)port;

        return true;
    }

    auto World::GetId() const -> int8_t
    {
        return _id;
    }

    auto World::GetAddress() const -> const std::string&
    {
        return _address;
    }
}
