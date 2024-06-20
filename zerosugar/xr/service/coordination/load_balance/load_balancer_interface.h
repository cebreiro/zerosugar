#pragma once
#include "zerosugar/xr/service/coordination/node/node_id.h"

namespace zerosugar::xr::coordination
{
    struct ServerStatus;
    class GameServer;
    class GameInstance;

    class ILoadBalancer
    {
    public:
        virtual ~ILoadBalancer() = default;

        virtual bool Add(const game_server_id_type& id) = 0;
        virtual bool Update(const game_server_id_type& id, const ServerStatus& status) = 0;
        virtual bool Remove(const game_server_id_type& id) = 0;

        virtual auto Select() -> GameServer* = 0;
    };
}
