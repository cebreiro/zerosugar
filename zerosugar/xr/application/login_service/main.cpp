#include <boost/lexical_cast.hpp>

#include "zerosugar/shared/app/app_intance.h"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger_builder.h"
#include "zerosugar/xr/service/login/login_service.h"
#include "zerosugar/xr/service/model/generated/database_service.h"

namespace zerosugar::xr
{
    class LoginServiceApplication final : public AppInstance
    {
    public:
        LoginServiceApplication();

        auto GetName() const->std::string_view override;

    private:
        void OnStartUp(std::span<char*> args) override;
        void OnShutdown() override;

    private:
        SharedPtrNotNull<execution::AsioExecutor> _executor;
        SharedPtrNotNull<RPCClient> _rpcClient;
        SharedPtrNotNull<LoginService> _loginService;
        SharedPtrNotNull<service::DatabaseServiceProxy> _databaseServiceProxy;
    };

    LoginServiceApplication::LoginServiceApplication()
        : _executor(std::make_shared<execution::AsioExecutor>(4))
        , _rpcClient(std::make_shared<RPCClient>(_executor))
        , _loginService(std::make_shared<LoginService>(_executor))
        , _databaseServiceProxy(std::make_shared<service::DatabaseServiceProxy>(_rpcClient))
    {
        ExecutionContext::PushExecutor(_executor.get());
    }

    auto LoginServiceApplication::GetName() const -> std::string_view
    {
        return "login_service_application";
    }

    void LoginServiceApplication::OnStartUp(std::span<char*> args)
    {
        ServiceLocator& serviceLocator = GetServiceLocator();

        SpdLogLoggerBuilder builder;
        builder.ConfigureConsole().SetLogLevel(LogLevel::Info).SetAsync(false);

        auto temp = std::make_shared<LogService>();
        temp->Add(-1, builder.CreateLogger());

        serviceLocator.Add<ILogService>(std::move(temp));
        serviceLocator.Add<RPCClient>(_rpcClient);
        serviceLocator.Add<service::IDatabaseService>(_databaseServiceProxy);

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

        _loginService->Initialize(GetServiceLocator());

        _rpcClient->RegisterToServer(std::string(_loginService->GetName())).Get();
    }

    void LoginServiceApplication::OnShutdown()
    {

    }
}

int main(int argc, char* argv[])
{
    using namespace zerosugar::xr;

    LoginServiceApplication application;

    return application.Run(std::span(argv, argc));
}
