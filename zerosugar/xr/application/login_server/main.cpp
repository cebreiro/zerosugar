#include <boost/lexical_cast.hpp>

#include "zerosugar/shared/app/app_intance.h"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger_builder.h"
#include "zerosugar/xr/server/login/login_server.h"
#include "zerosugar/xr/service/model/generated/gateway_service.h"
#include "zerosugar/xr/service/model/generated/login_service.h"

namespace zerosugar::xr
{
    class LoginServerApplication final : public AppInstance
    {
    public:
        LoginServerApplication();

        auto GetName() const->std::string_view override;

    private:
        void OnStartUp(std::span<char*> args) override;
        void OnShutdown() override;

    private:
        SharedPtrNotNull<execution::AsioExecutor> _executor;
        SharedPtrNotNull<RPCClient> _rpcClient;
        SharedPtrNotNull<LoginServer> _loginServer;
        SharedPtrNotNull<service::LoginServiceProxy> _loginServiceProxy;
        SharedPtrNotNull<service::GatewayServiceProxy> _gatewayServiceProxy;
    };

    LoginServerApplication::LoginServerApplication()
        : _executor(std::make_shared<execution::AsioExecutor>(4))
        , _rpcClient(std::make_shared<RPCClient>(_executor))
        , _loginServer(std::make_shared<LoginServer>(*_executor))
        , _loginServiceProxy(std::make_shared<service::LoginServiceProxy>(_rpcClient))
        , _gatewayServiceProxy(std::make_shared<service::GatewayServiceProxy>(_rpcClient))
    {
        ExecutionContext::PushExecutor(_executor.get());
    }

    auto LoginServerApplication::GetName() const -> std::string_view
    {
        return "login_server_application";
    }

    void LoginServerApplication::OnStartUp(std::span<char*> args)
    {
        ServiceLocator& serviceLocator = GetServiceLocator();

        SpdLogLoggerBuilder builder;
        builder.ConfigureConsole().SetLogLevel(LogLevel::Info).SetAsync(false);

        auto temp = std::make_shared<LogService>();
        temp->Add(-1, builder.CreateLogger());

        serviceLocator.Add<ILogService>(std::move(temp));
        serviceLocator.Add<service::ILoginService>(_loginServiceProxy);
        serviceLocator.Add<service::IGatewayService>(_gatewayServiceProxy);

        ZEROSUGAR_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] command lines args: {}",
                GetName(), [&]() -> std::string
                {
                    std::ostringstream oss;

                    for (const char* arg : args)
                    {
                        oss << arg << ' ';
                    }

                    std::string result = oss.str();
                    if (!result.empty())
                    {
                        result.pop_back();
                    }

                    return result;
                }()));

        if (args.size() < 3)
        {
            throw std::runtime_error("command line { args[1], args[2] } -> rpc server endpoint is null");
        }

        const std::string address(args[1]);
        const uint16_t port = boost::lexical_cast<uint16_t>(args[2]);

        _executor->Run();

        Future<void> future = _rpcClient->ConnectAsync(address, port);

        auto lastTimePoint = std::chrono::system_clock::now();

        while (future.IsPending())
        {
            const auto now = std::chrono::system_clock::now();

            if (now - lastTimePoint >= std::chrono::seconds(3))
            {
                ZEROSUGAR_LOG_INFO(GetServiceLocator(),
                    fmt::format("[{}] try to connect rpc server...",
                        GetName()));

                lastTimePoint = now;
            }
        }

        _loginServer->Initialize(GetServiceLocator());
        _loginServer->StartUp(8181);
    }

    void LoginServerApplication::OnShutdown()
    {
        
    }
}

int main(int argc, char* argv[])
{
    using namespace zerosugar::xr;

    LoginServerApplication application;

    return application.Run(std::span(argv, argc));
}
