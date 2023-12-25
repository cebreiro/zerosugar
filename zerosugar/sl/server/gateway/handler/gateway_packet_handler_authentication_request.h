#pragma once
#include "zerosugar/sl/protocol/packet/generated/gateway_cs_generated.h"
#include "zerosugar/sl/server/gateway/handler/gateway_packet_handler_interface.h"

namespace zerosugar::sl::detail
{
    class GatewayPacketHandler_AuthenticationRequest final : public GatewayPacketHandlerT<gateway::cs::AuthenticationRequest>
    {
    public:
        explicit GatewayPacketHandler_AuthenticationRequest(WeakPtrNotNull<GatewayServer> server);

        auto HandlePacket(GatewayServer& server, GatewayClient& client,
            const gateway::cs::AuthenticationRequest& packet) const->Future<void> override;
    };
}
