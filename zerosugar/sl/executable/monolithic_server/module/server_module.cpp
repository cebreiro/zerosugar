#include "server_module.h"

#include "zerosugar/shared/app/app_intance.h"
#include "zerosugar/sl/server/login/login_server.h"

namespace zerosugar::sl
{
    void ServerModule::Initialize(AppInstance& app, ServerConfig& config)
    {
        (void)config;

        const auto concurrency = static_cast<int64_t>(std::thread::hardware_concurrency());
        _executor = std::make_shared<execution::AsioExecutor>(concurrency);
        _executor->Run();

        auto loginServer = std::make_shared<LoginServer>(*_executor, app.GetServiceLocator());
        loginServer->StartUp();

        _loginServer = std::move(loginServer);
    }

    void ServerModule::Finalize() noexcept
    {
        if (_loginServer)
        {
            _loginServer->Shutdown();
        }

        if (_executor)
        {
            _executor->Stop();
        }
    }

    void ServerModule::GetFinalizeError(std::vector<boost::system::error_code>& errors)
    {
        if (_executor)
        {
            _executor->Join(&errors);
        }
    }
}
