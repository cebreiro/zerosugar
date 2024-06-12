#pragma once
#include <cstdint>
#include "zerosugar/sl/service/generated/world_service_generated.h"
#include "zerosugar/sl/service/generated/shared_generated.h"
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/execution/channel/channel.h"

namespace zerosugar::sl::service
{
    class IWorldService : public IService
    {
    public:
        virtual ~IWorldService() = default;

        virtual auto CreateWorldAsync(CreateWorldParam param) -> Future<CreateWorldResult> = 0;
        virtual auto CreateZoneAsync(CreateZoneParam param) -> Future<CreateZoneResult> = 0;
        virtual auto GetWorldListAsync(GetWorldListParam param) -> Future<GetWorldListResult> = 0;
        virtual auto EnterWorldAsync(EnterWorldParam param) -> Future<EnterWorldResult> = 0;
        virtual auto KickPlayerAsync(KickPlayerParam param) -> Future<KickPlayerResult> = 0;
    };
}
