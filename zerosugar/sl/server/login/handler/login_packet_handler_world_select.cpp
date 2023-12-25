#include "login_packet_handler_world_select.h"

#include <boost/lexical_cast.hpp>

#include "zerosugar/sl/protocol/packet/login/sc/world_enter.h"
#include "zerosugar/sl/server/login/login_client.h"
#include "zerosugar/sl/server/login/login_server.h"

namespace zerosugar::sl::detail
{
    LoginPacketHandler_WorldSelect::LoginPacketHandler_WorldSelect(WeakPtrNotNull<LoginServer> server)
        : LoginPacketHandlerT(std::move(server))
    {
        AddAllowedState(LoginClientState::Authenticated);
    }

    auto LoginPacketHandler_WorldSelect::HandlePacket(LoginServer& server,
        LoginClient& client, const login::cs::WorldSelect& packet) const -> Future<void>
    {
        (void)packet;

        using namespace service;

        auto* worldService = server.GetLocator().Find<IWorldService>();
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

        const uint64_t key = boost::lexical_cast<uint64_t>(client.GetAuthToken());
        const int32_t key1 = static_cast<int32_t>((key & 0x00000000FFFFFFFF));
        const int32_t key2 = static_cast<int32_t>((key & 0xFFFFFFFF00000000) >> 32);

        client.SetState(LoginClientState::WorldEntranced);
        client.SendPacket(login::sc::WorldEnter(key1, key2, packet.GetWorldId()));
        //client.Close(std::chrono::seconds(3));

        co_return;
    }
}
