#pragma once
#include "zerosugar/sl/protocol/packet/generated/gateway_cs_generated.h"
#include "zerosugar/sl/server/gateway/handler/gateway_packet_handler_interface.h"

namespace zerosugar::sl::detail
{
    class GatewayPacketHandler_VersionCheck final : public GatewayPacketHandlerT<gateway::cs::VersionCheckRequest>
    {
    public:
        explicit GatewayPacketHandler_VersionCheck(WeakPtrNotNull<GatewayServer> server);

        auto HandlePacket(GatewayServer& server, GatewayClient& client,
            const gateway::cs::VersionCheckRequest& packet) const -> Future<void> override;
    };
}
