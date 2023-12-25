#include "gateway_packet_handler_version_check.h"

#include "zerosugar/sl/protocol/packet/generated/gateway_sc_generated.h"
#include "zerosugar/sl/server/gateway/gateway_client.h"
#include "zerosugar/sl/server/gateway/gateway_server.h"

namespace zerosugar::sl::detail
{
    GatewayPacketHandler_VersionCheck::GatewayPacketHandler_VersionCheck(WeakPtrNotNull<GatewayServer> server)
        : GatewayPacketHandlerT(std::move(server))
    {
        AddAllowedState(GatewayClientState::Connected);
    }

    auto GatewayPacketHandler_VersionCheck::HandlePacket(GatewayServer& server, GatewayClient& client,
        const gateway::cs::VersionCheckRequest& packet) const -> Future<void>
    {
        constexpr int32_t gameVersion = 2;

        if (packet.version != gameVersion)
        {
            ZEROSUGAR_LOG_CRITICAL(server.GetLocator(),
                std::format("[{}] invalid client game version. client: {}", GetName(), client.ToString()));
        }

        gateway::sc::VersionCheckResponse response;
        response.fail = false;

        client.SetState(GatewayClientState::VersionChecked);
        client.SendPacket(response);

        co_return;
    }
}
