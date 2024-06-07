#pragma once
#include "zerosugar/sl/protocol/packet/generated/gateway_cs_generated.h"
#include "zerosugar/sl/server/gateway/handler/gateway_packet_handler_interface.h"
#include "zerosugar/sl/service/generated/shared_generated.h"

namespace zerosugar::sl::detail
{
    class GatewayPacketHandler_CharacterList final : public GatewayPacketHandlerT<gateway::cs::CharacterListRequest>
    {
    public:
        explicit GatewayPacketHandler_CharacterList(WeakPtrNotNull<GatewayServer> server);

        auto HandlePacket(GatewayServer& server, GatewayClient& client,
            const gateway::cs::CharacterListRequest& packet) const->Future<void> override;

    private:
        static auto Convert(const service::Character& character) -> gateway::Character;
    };
}
