#include "login_packet_handler_interface.h"

#include "zerosugar/sl/server/login/login_client.h"

namespace zerosugar::sl::detail
{
    LoginPacketHandlerAbstract::LoginPacketHandlerAbstract(WeakPtrNotNull<LoginServer> server)
        : _server(std::move(server))
    {
    }

    bool LoginPacketHandlerAbstract::CanHandle(const LoginClient& client) const
    {
        if (_allowedStates.empty())
        {
            return true;
        }

        return _allowedStates.contains(client.GetState());
    }

    void LoginPacketHandlerAbstract::AddAllowedState(LoginClientState state)
    {
        _allowedStates.insert(state);
    }

    void LoginPacketHandlerAbstract::AddAllowedStateRange(std::initializer_list<LoginClientState> states)
    {
        _allowedStates.insert(states.begin(), states.end());
    }
}
