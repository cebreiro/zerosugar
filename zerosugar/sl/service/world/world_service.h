#pragma once
#include <tbb/concurrent_unordered_map.h>
#include "zerosugar/sl/service/generated/repository_service_generated_interface.h"
#include "zerosugar/sl/service/generated/world_service_generated_interface.h"

namespace zerosugar::sl
{
    class World;

    class WorldService final
        : public service::IWorldService
        , public std::enable_shared_from_this<WorldService>
    {
    public:
        using Locator = ServiceLocatorRef<ILogService, service::IRepositoryService>;

    public:
        WorldService() = delete;

        explicit WorldService(execution::IExecutor& executor);
        ~WorldService() override;

        void Initialize(ServiceLocator& dependencyLocator) override;

        auto CreateWorldAsync(service::CreateWorldParam param) -> Future<service::CreateWorldResult> override;
        auto CreateZoneAsync(service::CreateZoneParam param) -> Future<service::CreateZoneResult> override;
        auto GetWorldListAsync(service::GetWorldListParam param) -> Future<service::GetWorldListResult> override;
        auto EnterWorldAsync(service::EnterWorldParam param) -> Future<service::EnterWorldResult> override;
        auto KickPlayerAsync(service::KickPlayerParam param) -> Future<service::KickPlayerResult> override;

    private:
        std::atomic<bool> _shutdown = false;
        execution::IExecutor& _executor;
        Locator _locator;

        tbb::concurrent_unordered_map<int8_t, UniquePtrNotNull<World>> _worlds;
    };
}
