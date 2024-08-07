#pragma once
#include "zerosugar/xr/server/game/instance/entity/game_entity_serializer_interface.h"

namespace zerosugar::xr
{
    class MapData;
}

namespace zerosugar::xr
{
    class GameEntitySerializer final : public IGameEntitySerializer
    {
    public:
        explicit GameEntitySerializer(const MapData& data);

        auto Serialize(const GameEntity& entity) const -> service::DTOCharacter override;
        auto Deserialize(const service::DTOCharacter& character) const -> SharedPtrNotNull<GameEntity> override;

    private:
        const MapData& _mapData;
    };
}
