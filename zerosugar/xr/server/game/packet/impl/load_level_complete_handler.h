#pragma once
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/packet/packet_handler_interface.h"

namespace zerosugar::xr
{
    class LoadLevelCompleteHandler final : public IGamePacketHandlerT<network::game::cs::LoadLevelComplete>
    {
    public:
        auto HandlePacket(GameServer& server, Session& session,
            UniquePtrNotNull<network::game::cs::LoadLevelComplete> packet) -> Future<void> override;
    };
}
