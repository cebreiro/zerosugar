#include "login_packet_handler_world_select.h"

#include "zerosugar/sl/protocol/packet/login/sc/world_list.h"
#include "zerosugar/sl/server/login/login_client.h"
#include "zerosugar/sl/server/login/login_server.h"

namespace zerosugar::sl::detail
{
    LoginPacketHandler_WorldSelect::LoginPacketHandler_WorldSelect()
    {
        AddAllowedState(LoginClientState::Authenticated);
    }

    auto LoginPacketHandler_WorldSelect::HandlePacket(const LoginServer& server, LoginClient& client,
        const login::cs::WorldSelect& packet) const -> Future<void>
    {
        (void)server;
        (void)packet;

        using namespace service;

        auto& serviceLocator = client.GetLocator();
        (void)serviceLocator;

        auto* worldService = serviceLocator.Find<IWorldService>();
        if (!worldService)
        {
            co_return;
        }

        const EnterWorldParam param{
            .worldId = packet.GetWorldId(),
            .token = AuthToken{.values = client.GetAuthToken(), },
            .accountId = client.GetAccountId(),
            .account = client.GetAccount(),
        };
        const EnterWorldResult& result = co_await worldService->EnterWorldAsync(param);
        if (result.errorCode != WorldServiceErrorCode::WorldErrorNone)
        {
            co_return;
        }

        client.SetState(LoginClientState::WorldEntranced);
        client.Close(std::chrono::seconds(3));

        co_return;
    }
}
