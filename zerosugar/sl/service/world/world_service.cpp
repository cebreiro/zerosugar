#include "world_service.h"

#include "zerosugar/sl/service/world/world.h"

namespace zerosugar::sl
{
    WorldService::WorldService(execution::IExecutor& executor)
        : _executor(executor)
    {
    }

    WorldService::~WorldService()
    {
    }

    void WorldService::Initialize(ServiceLocator& serviceLocator)
    {
        _locator = serviceLocator;
    }

    auto WorldService::CreateWorldAsync(service::CreateWorldParam param) -> Future<service::CreateWorldResult>
    {
        auto world = std::make_unique<World>(_executor, param.worldId, param.address);
        const bool success = _worlds.emplace(std::make_pair(param.worldId, std::move(world))).second;

        co_return service::CreateWorldResult{
            .errorCode = success ?
                service::WorldServiceErrorCode::WorldErrorNone :
                service::WorldServiceErrorCode::WorldErrorWorldAlreadyExists,
        };
    }

    auto WorldService::CreateZoneAsync(service::CreateZoneParam param) -> Future<service::CreateZoneResult>
    {
        auto iter = _worlds.find(static_cast<int8_t>(param.worldId));
        if (iter != _worlds.end())
        {
            const bool success = iter->second->CreateZone(param.zoneId, param.address, static_cast<uint16_t>(param.port));

            co_return service::CreateZoneResult{
                .errorCode = success ?
                    service::WorldServiceErrorCode::WorldErrorNone :
                    service::WorldServiceErrorCode::WorldErrorZoneAlreadyExists,
            };
        }

        co_return service::CreateZoneResult{
            .errorCode = service::WorldServiceErrorCode::WorldErrorNone,
        };
    }

    auto WorldService::GetWorldListAsync(service::GetWorldListParam param) -> Future<service::GetWorldListResult>
    {
        std::vector<service::World> worlds;
        worlds.reserve(_worlds.size());

        for (const World& world : _worlds | std::views::values | notnull::reference)
        {
            worlds.emplace_back(service::World{
                .worldId = world.GetId(),
                .address = world.GetAddress(),
                });
        }

        co_return service::GetWorldListResult{
            .errorCode = service::WorldServiceErrorCode::WorldErrorNone,
            .worlds = std::move(worlds),
        };
    }

    auto WorldService::EnterWorldAsync(service::EnterWorldParam param) -> Future<service::EnterWorldResult>
    {
        (void)param;

        co_return{};
    }

    auto WorldService::KickPlayerAsync(service::KickPlayerParam param) -> Future<service::KickPlayerResult>
    {
        (void)param;

        co_return{};
    }
}
