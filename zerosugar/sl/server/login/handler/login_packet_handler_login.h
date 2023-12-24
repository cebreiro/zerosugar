#pragma once
#include "zerosugar/sl/protocol/packet/login/cs/login.h"
#include "zerosugar/sl/server/login/handler/login_packet_handler_interface.h"

namespace zerosugar::sl::login::sc
{
    enum class LoginFailReason;
}

namespace zerosugar::sl::detail
{
    class LoginPacketHandler_Login final : public LoginPacketHandlerT<login::cs::Login>
    {
    public:
        explicit LoginPacketHandler_Login(WeakPtrNotNull<LoginServer> server);

        auto HandlePacket(LoginServer& server, LoginClient& client, const login::cs::Login& packet) const
            -> Future<void> override;

    private:
        static void ProcessFailure(LoginClient& client, login::sc::LoginFailReason reason);
    };
}
