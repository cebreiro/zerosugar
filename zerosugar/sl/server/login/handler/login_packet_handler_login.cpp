#include "login_packet_handler_login.h"

#include "zerosugar/sl/protocol/packet/login/sc/login_fail.h"
#include "zerosugar/sl/protocol/packet/login/sc/world_list.h"
#include "zerosugar/sl/server/login/login_client.h"
#include "zerosugar/sl/server/login/login_server.h"
#include "zerosugar/sl/service/generated/world_service_generated_interface.h"

namespace zerosugar::sl::detail
{
    LoginPacketHandler_Login::LoginPacketHandler_Login()
    {
        AddAllowedState(LoginClientState::Connected);
    }

    auto LoginPacketHandler_Login::HandlePacket(const LoginServer& server, LoginClient& client, const login::cs::Login& packet) const
        -> Future<void>
    {
        using namespace service;

        auto& serviceLocator = client.GetLocator();

        auto* loginService = serviceLocator.Find<ILoginService>();
        auto* worldService = serviceLocator.Find<IWorldService>();

        if (!loginService || !worldService)
        {
            ProcessFailure(client, login::sc::LoginFailReason::CantConnectServer);

            ZEROSUGAR_LOG_ERROR(serviceLocator, std::format("[{}] fail to find service",
                GetName(), client));

            co_return;
        }

        const LoginResult loginResult = co_await loginService->LoginAsync(LoginParam{
            .account = packet.GetAccount(),
            .password = packet.GetPassword(),
            .context = std::format("[{}] client: {}", GetName(), client),
        });

        if (loginResult.errorCode != LoginServiceErrorCode::LoginErrorNone)
        {
            if (loginResult.errorCode == LoginServiceErrorCode::LoginErrorFailDuplicateLogin)
            {
                (void)loginService->KickAsync(KickParam{
                    .account = packet.GetAccount(),
                    .context = std::format("[{}] client: {}", GetName(), client),
                    });

                ProcessFailure(client, login::sc::LoginFailReason::ReleaseExistingConnection);
            }
            else if (loginResult.errorCode == LoginServiceErrorCode::LoginErrorFailInvalid)
            {
                ProcessFailure(client, login::sc::LoginFailReason::InvalidIDPassword);
            }

            co_return;
        }

        client.SetState(LoginClientState::Authenticated);
        client.SetAuthToken(loginResult.token.values);
        client.SetAccountId(loginResult.accountId);
        client.SetAccount(packet.GetAccount());

        GetWorldListResult getWorldResult =
            co_await serviceLocator.Find<IWorldService>()->GetWorldListAsync({});

        if (getWorldResult.errorCode != WorldServiceErrorCode::WorldErrorNone)
        {
            co_return;
        }

        const login::sc::WorldList worldList = [&]()
            {
                const auto transform = [src = server.GetPublicAddress()](const World& world) -> login::sc::World
                    {
                        return login::sc::World::Construct(static_cast<int8_t>(world.worldId), src, world.address);
                    };
                return login::sc::WorldList(getWorldResult.worlds | std::views::transform(transform) | std::ranges::to<std::vector>());
            }();

        client.SendPacket(worldList);
    }

    void LoginPacketHandler_Login::ProcessFailure(LoginClient& client, login::sc::LoginFailReason reason)
    {
        client.SendPacket(login::sc::LoginFail(reason));
        client.Close(std::chrono::seconds(3));
    }
}
