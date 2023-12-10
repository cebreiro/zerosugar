#include "login_packet_handler_login.h"

#include "zerosugar/sl/server/client/client.h"

namespace zerosugar::sl
{
    auto LoginPacketHandler_Login::HandlePacket(Client& client, const login::cs::Login& packet) const
        -> Future<void>
    {
        auto* loginService = client.GetLocator().Find<service::ILoginService>();
        if (!loginService)
        {
            client.Close();

            ZEROSUGAR_LOG_ERROR(client.GetLocator(), std::format("[] fail to find login service",
                GetName(), client));

            co_return;
        }

        using namespace service;

        auto result = co_await loginService->LoginAsync(service::LoginParam{
            .account = packet.GetAccount(),
            .password = packet.GetPassword(),
            .context = std::format("[{}] client: {}", GetName(), client),
            });

        if (result.errorCode != LoginServiceErrorCode::LoginErrorNone)
        {
            
        }

        co_return;
    }
}
