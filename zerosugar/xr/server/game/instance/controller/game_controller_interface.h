#pragma once
#include "zerosugar/xr/server/game/instance/controller/game_controller_id.h"

namespace zerosugar::xr
{
    class GameEntity;
    class IPacket;
}

namespace zerosugar::xr
{
    class IGameController
    {
    public:
        virtual ~IGameController() = default;

        virtual void Notify(const IPacket& packet) = 0;

        virtual auto GetControllerId() const -> game_controller_id_type = 0;
        virtual void SetControllerId(game_controller_id_type id) = 0;
    };
}
