#include "gateway_packet_handler_authentication_request.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/sl/protocol/packet/generated/gateway_sc_generated.h"
#include "zerosugar/sl/server/gateway/gateway_client.h"
#include "zerosugar/sl/server/gateway/gateway_server.h"

namespace zerosugar::sl::detail
{
    GatewayPacketHandler_AuthenticationRequest::GatewayPacketHandler_AuthenticationRequest(
        WeakPtrNotNull<GatewayServer> server)
        : GatewayPacketHandlerT(std::move(server))
    {
        AddAllowedState(GatewayClientState::VersionChecked);
    }

    auto GatewayPacketHandler_AuthenticationRequest::HandlePacket(GatewayServer& server, GatewayClient& client,
        const gateway::cs::AuthenticationRequest& packet) const -> Future<void>
    {
        const uint64_t key = ((static_cast<uint64_t>(packet.key1) << 32) & 0xFFFFFFFF00000000) |( static_cast<uint64_t>(packet.key2) & 0x00000000FFFFFFFF);

        using namespace service;

        auto& serviceLocator = server.GetLocator();

        const AuthenticateParam param{
            .token = AuthToken{ .values = std::to_string(key) },
            .address = client.GetSession().GetRemoteAddress(),
        };

        const AuthenticateResult& result = co_await serviceLocator.Get<ILoginService>().AuthenticateAsync(param);
        if (result.errorCode != LoginServiceErrorCode::LoginErrorNone)
        {
            ZEROSUGAR_LOG_ERROR(serviceLocator, std::format("[{}] fail to authenticate. client: {}", GetName(), client.ToString()));

            gateway::sc::AuthenticationResponse response;
            response.fail = true;

            client.SendPacket(response);
            client.Close(std::chrono::seconds(3));

            co_return;
        }

        client.SetState(GatewayClientState::Authenticated);
        client.SetAuthToken(param.token.values);
        client.SetAccountId(result.accountId);

        gateway::sc::AuthenticationResponse response;
        response.fail = false;
        response.unkEssentialString = "unknown...";

        client.SendPacket(response);
    }
}
