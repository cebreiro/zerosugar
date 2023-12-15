#include "login_packet_handler_login.h"

#include "zerosugar/sl/executable/monolithic_server/config/server_config.h"
#include "zerosugar/sl/protocol/packet/login/sc/world_list.h"
#include "zerosugar/sl/server/login/login_client.h"
#include "zerosugar/sl/server/login/login_server.h"

namespace zerosugar::sl
{
    auto LoginPacketHandler_Login::HandlePacket(const LoginServer& server, LoginClient& client, const login::cs::Login& packet) const
        -> Future<void>
    {
        using namespace service;

        auto* loginService = client.GetLocator().Find<ILoginService>();
        if (!loginService)
        {
            client.Close();

            ZEROSUGAR_LOG_ERROR(client.GetLocator(), std::format("[] fail to find login service",
                GetName(), client));

            co_return;
        }

        LoginResult result = co_await loginService->LoginAsync(LoginParam{
            .account = packet.GetAccount(),
            .password = packet.GetPassword(),
            .context = std::format("[{}] client: {}", GetName(), client),
        });

        if (result.errorCode != LoginServiceErrorCode::LoginErrorNone)
        {
            co_return;
        }

        client.SetState(LoginClientState::Authenticated);

        const std::string& address = server.GetConfig().GetAddress();
        const std::vector<login::sc::World> worldList = server.GetConfig().GetWorldConfigs() | std::views::transform(
            [&address](const WorldConfig& config) -> login::sc::World
            {
                return login::sc::World::Construct(config.GetWorldId(), address, address);
            }) | std::ranges::to<std::vector>();


        client.SendPacket(login::sc::WorldList(worldList));

        co_return;
    }
}
