#include "game_instance.h"

namespace zerosugar::xr
{
    GameInstance::GameInstance(SharedPtrNotNull<execution::IExecutor> executor, service_locator_type serviceLocator,
        game_instance_id_type id, int32_t zoneId)
        : _strand(std::make_shared<Strand>(std::move(executor)))
        , _serviceLocator(std::move(serviceLocator))
        , _id(id)
        , _zoneId(zoneId)
    {
    }

    auto GameInstance::Accept(SharedPtrNotNull<GameClient> client) -> Future<void>
    {
        (void)client;

        co_return;
    }

    auto GameInstance::GetId() const -> game_instance_id_type
    {
        return _id;
    }

    auto GameInstance::GetZoneId() const -> int32_t
    {
        return _zoneId;
    }
}
