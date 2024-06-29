#pragma once
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/packet/packet_handler_interface.h"

namespace zerosugar::xr
{
    class StopPlayerHandler final : public IGamePacketHandlerT<network::game::cs::StopPlayer>
    {
    public:
        auto HandlePacket(GameServer& server, Session& session, const network::game::cs::StopPlayer& packet) -> Future<void> override;
    };
}
