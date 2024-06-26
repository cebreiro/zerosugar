#pragma once
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/packet/packet_handler_interface.h"

namespace zerosugar::xr
{
    class AuthenticateHandler final : public IGamePacketHandlerT<network::game::cs::Authenticate>
    {
    public:
        auto HandlePacket(GameServer& server, Session& session,
            const network::game::cs::Authenticate& packet) const -> Future<void> override;
    };
}
