#pragma once
#include "zerosugar/sl/protocol/packet/login/cs/world_select.h"
#include "zerosugar/sl/server/login/handler/login_packet_handler_interface.h"

namespace zerosugar::sl::detail
{
    class LoginPacketHandler_WorldSelect : public LoginPacketHandlerT<login::cs::WorldSelect>
    {
    public:
        LoginPacketHandler_WorldSelect();

        auto HandlePacket(const LoginServer& server, LoginClient& client, const login::cs::WorldSelect& packet) const
            -> Future<void> override;
    };
}
