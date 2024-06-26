#pragma once
#include "zerosugar/xr/service/model/generated/data_transfer_object_message.h"

namespace zerosugar::xr
{
    class GameEntity;

    class IGameEntitySerializer
    {
    public:
        virtual ~IGameEntitySerializer() = default;

        virtual auto Serialize(const GameEntity& entity) const ->service::DTOCharacter = 0;
        virtual auto Deserialize(const service::DTOCharacter& character) const -> SharedPtrNotNull<GameEntity> = 0;
    };
}
