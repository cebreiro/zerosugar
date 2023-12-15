#pragma once
#include "zerosugar/sl/protocol/packet/login/cs/login.h"
#include "zerosugar/sl/server/login/handler/login_packet_handler_interface.h"

namespace zerosugar::sl
{
    class LoginPacketHandler_Login final : public LoginPacketHandlerT<login::cs::Login>
    {
    public:
        auto HandlePacket(const LoginServer& server, LoginClient& client, const login::cs::Login& packet) const
            -> Future<void> override;
    };
}
