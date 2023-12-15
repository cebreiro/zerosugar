#pragma once
#include "zerosugar/shared/log/log_service_interface.h"
#include "zerosugar/shared/service/service_locator.h"
#include "zerosugar/shared/network/server/server.h"
#include "zerosugar/sl/server/login/login_client_id.h"
#include "zerosugar/sl/service/generated/login_service_generated_interface.h"

namespace zerosugar::sl
{
    class ServerConfig;

    class LoginServer final : public Server
    {
        using Server::StartUp;

    public:
        static constexpr uint16_t PORT = 2106;

        using locator_type = ServiceLocatorRef<ILogService, service::ILoginService>;

    public:
        LoginServer() = delete;
        LoginServer(const LoginServer& other) = delete;
        LoginServer(LoginServer&& other) noexcept = delete;
        LoginServer& operator=(const LoginServer& other) = delete;
        LoginServer& operator=(LoginServer&& other) noexcept = delete;

        LoginServer(execution::AsioExecutor& executor, locator_type locator, const ServerConfig& config);
        ~LoginServer();

        void StartUp();
        void Shutdown() override;

        auto GetConfig() const -> const ServerConfig&;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

    private:
        locator_type _locator;
        const ServerConfig& _config;
        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<class LoginClient>> _clients;
        std::atomic<login_client_id_type::value_type> _nextClientId = 0;
    };
}
