#pragma once
#include "zerosugar/xr/server/game/instance/game_instance_id.h"

namespace zerosugar::xr
{
    class GameClient;

    class GameInstance final
        : public std::enable_shared_from_this<GameInstance>
    {
    public:
        using service_locator_type = ServiceLocatorT<ILogService>;

    public:
        GameInstance(SharedPtrNotNull<execution::IExecutor> executor, service_locator_type serviceLocator,
            game_instance_id_type id, int32_t zoneId);

        auto Accept(SharedPtrNotNull<GameClient> client) -> Future<void>;

        auto GetId() const -> game_instance_id_type;
        auto GetZoneId() const -> int32_t;

    private:
        SharedPtrNotNull<Strand> _strand;
        service_locator_type _serviceLocator;
        game_instance_id_type _id;
        int32_t _zoneId = 0;
    };
}
