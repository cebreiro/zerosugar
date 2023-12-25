#include "gateway_packet_handler_interface.h"

#include "zerosugar/sl/server/gateway/gateway_client.h"

namespace zerosugar::sl::detail
{
    GatewayPacketHandlerAbstract::GatewayPacketHandlerAbstract(WeakPtrNotNull<GatewayServer> server)
        : _server(std::move(server))
    {
    }

    bool GatewayPacketHandlerAbstract::CanHandle(const GatewayClient& client) const
    {
        if (_allowedStates.empty())
        {
            return true;
        }

        return _allowedStates.contains(client.GetState());
    }

    void GatewayPacketHandlerAbstract::AddAllowedState(GatewayClientState state)
    {
        _allowedStates.insert(state);
    }

    void GatewayPacketHandlerAbstract::AddAllowedStateRange(std::initializer_list<GatewayClientState> states)
    {
        _allowedStates.insert(states.begin(), states.end());
    }
}
