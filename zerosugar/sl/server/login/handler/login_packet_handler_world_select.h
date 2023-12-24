#pragma once
#include "zerosugar/sl/protocol/packet/login/cs/world_select.h"
#include "zerosugar/sl/server/login/handler/login_packet_handler_interface.h"

namespace zerosugar::sl::detail
{
    class LoginPacketHandler_WorldSelect : public LoginPacketHandlerT<login::cs::WorldSelect>
    {
    public:
        explicit  LoginPacketHandler_WorldSelect(WeakPtrNotNull<LoginServer> server);

        auto HandlePacket(LoginServer& server, LoginClient& client, const login::cs::WorldSelect& packet) const
            -> Future<void> override;
    };
}
