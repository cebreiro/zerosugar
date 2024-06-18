#pragma once
#include "zerosugar/xr/service/coordination/node/node.h"
#include "zerosugar/xr/service/coordination/node/node_id.h"

namespace zerosugar::xr::coordination
{
    class GameServer;
    class GameUser;

    class GameInstance : public Node<GameServer*, GameUser*, game_user_id_type>
    {
    public:
        GameInstance(game_instance_id_type id, int32_t zoneId);

        auto GetId() const -> const game_instance_id_type&;
        auto GetZoneId() const -> int32_t;

    private:
        game_instance_id_type _id;
        int32_t _zoneId;
    };
}
