#include <boost/lexical_cast.hpp>

#include "zerosugar/shared/app/app_intance.h"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/execution/executor/impl/game_executor.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger_builder.h"
#include "zerosugar/xr/data/game_data_provider.h"
#include "zerosugar/xr/navigation/navi_data_provider.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/service/model/generated/login_service.h"
#include "zerosugar/xr/service/model/generated/database_service.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"

namespace zerosugar::xr
{
    class GameServerApplication final : public AppInstance
    {
    public:
        GameServerApplication();

        auto GetName() const->std::string_view override;

    private:
        void OnStartUp(std::span<char*> args) override;
        void OnShutdown() override;

    private:
        SharedPtrNotNull<execution::AsioExecutor> _ioExecutor;
        SharedPtrNotNull<execution::GameExecutor> _gameExecutor;
        SharedPtrNotNull<RPCClient> _rpcClient;
        SharedPtrNotNull<GameServer> _gameServer;

        SharedPtrNotNull<service::LoginServiceProxy> _loginServiceProxy;
        SharedPtrNotNull<service::DatabaseServiceProxy> _databaseServiceProxy;
        SharedPtrNotNull<service::CoordinationServiceProxy> _coordinationServiceProxy;

        SharedPtrNotNull<GameDataProvider> _gameDataProvider;
        SharedPtrNotNull<NavigationDataProvider> _navigationDataProvider;
    };

    GameServerApplication::GameServerApplication()
        : _ioExecutor(std::make_shared<execution::AsioExecutor>(8))
        , _gameExecutor(std::make_shared<execution::GameExecutor>(8))
        , _rpcClient(std::make_shared<RPCClient>(_ioExecutor))
        , _gameServer(std::make_shared<GameServer>(*_ioExecutor, *_gameExecutor))
        , _loginServiceProxy(std::make_shared<service::LoginServiceProxy>(_rpcClient))
        , _databaseServiceProxy(std::make_shared<service::DatabaseServiceProxy>(_rpcClient))
        , _coordinationServiceProxy(std::make_shared<service::CoordinationServiceProxy>(_rpcClient))
        , _gameDataProvider(std::make_shared<GameDataProvider>())
        , _navigationDataProvider(std::make_shared<NavigationDataProvider>())
    {
        ExecutionContext::PushExecutor(_ioExecutor.get());
    }

    auto GameServerApplication::GetName() const -> std::string_view
    {
        return "game_server_application";
    }

    void GameServerApplication::OnStartUp(std::span<char*> args)
    {
        ServiceLocator& serviceLocator = GetServiceLocator();

        SpdLogLoggerBuilder builder;
        builder.ConfigureConsole().SetLogLevel(LogLevel::Info).SetAsync(false);

        auto temp = std::make_shared<LogService>();
        temp->Add(-1, builder.CreateLogger());

        serviceLocator.Add<ILogService>(std::move(temp));
        serviceLocator.Add<service::ILoginService>(_loginServiceProxy);
        serviceLocator.Add<service::IDatabaseService>(_databaseServiceProxy);
        serviceLocator.Add<service::ICoordinationService>(_coordinationServiceProxy);
        serviceLocator.Add<GameDataProvider>(_gameDataProvider);
        serviceLocator.Add<NavigationDataProvider>(_navigationDataProvider);

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

        _ioExecutor->Run();
        _gameExecutor->Run();

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

        _gameDataProvider->Initialize(serviceLocator);
        _navigationDataProvider->InitializeData(serviceLocator, _gameDataProvider->GetBaseDirectory());

        _gameServer->Initialize(GetServiceLocator());
        _gameServer->SetPublicIP("127.0.0.1");
        _gameServer->StartUp(8183);
    }

    void GameServerApplication::OnShutdown()
    {

    }
}

int main(int argc, char* argv[])
{
    using namespace zerosugar::xr;

    GameServerApplication application;

    return application.Run(std::span(argv, argc));
}
