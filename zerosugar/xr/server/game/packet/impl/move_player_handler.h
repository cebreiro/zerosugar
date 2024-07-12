#pragma once
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/packet/packet_handler_interface.h"

namespace zerosugar::xr
{
    class MovePlayerHandler final : public IGamePacketHandlerT<network::game::cs::MovePlayer>
    {
    public:
        auto HandlePacket(GameServer& server, Session& session,
            UniquePtrNotNull<network::game::cs::MovePlayer> packet) ->Future<void> override;
    };
}
