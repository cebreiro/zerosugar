#pragma once
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/packet/packet_handler_interface.h"

namespace zerosugar::xr
{
    class StartPlayerAttackHandler final : public IGamePacketHandlerT<network::game::cs::StartPlayerAttack>
    {
    public:
        auto HandlePacket(GameServer& server, Session& session,
            UniquePtrNotNull<network::game::cs::StartPlayerAttack> packet) -> Future<void> override;
    };
}
