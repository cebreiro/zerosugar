#include "game_instance.h"

namespace zerosugar::xr::coordination
{
    GameInstance::GameInstance(game_instance_id_type id, int32_t zoneId)
        : _id(id)
        , _zoneId(zoneId)
    {
    }

    auto GameInstance::GetId() const -> const game_instance_id_type&
    {
        return _id;
    }

    auto GameInstance::GetZoneId() const -> int32_t
    {
        return _zoneId;
    }
}
