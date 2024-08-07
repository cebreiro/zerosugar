#pragma once
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/packet/packet_handler_interface.h"

namespace zerosugar::xr
{
    class ChatHandler final : public IGamePacketHandlerT<network::game::cs::Chat>
    {
    public:
        auto HandlePacket(GameServer& server, Session& session, std::unique_ptr<network::game::cs::Chat> packet) -> Future<void> override;
    };
}
