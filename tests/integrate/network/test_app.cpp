#include "test_app.h"

#include "tests/integrate/network/test_server.h"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/log/log_service.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger_builder.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger.h"

TestApp::TestApp()
    : _executor(std::make_shared<AsioExecutor>(static_cast<int64_t>(std::thread::hardware_concurrency())))
    , _logService(std::make_shared<zerosugar::LogService>())
    , _server(std::make_shared<TestServer>(*_executor, *this))
{
    zerosugar::SpdLogLoggerBuilder builder;
    builder.ConfigureConsole().SetLogLevel(zerosugar::LogLevel::Info);

    _logService->Add(-1, builder.CreateLogger());

    GetServiceLocator().Add<zerosugar::ILogService>(_logService);
}

auto TestApp::GetServer() -> TestServer&
{
    return *_server;
}

auto TestApp::GetExecutor() -> IExecutor&
{
    return *_executor;
}

auto TestApp::GetStrands() const -> const std::vector<SharedPtrNotNull<Strand>>&
{
    return _strands;
}

auto TestApp::GetName() const -> std::string_view
{
    return "integrate_network_test_app";
}

void TestApp::OnStartUp(std::span<char*> args)
{
    (void)args;

    _executor->Run();

    _server->Initialize(GetServiceLocator());
    _server->StartUp(PORT);

    for (int64_t i = 0; i < static_cast<int64_t>(std::thread::hardware_concurrency()); ++i)
    {
        _strands.emplace_back(std::make_shared<Strand>(_executor->SharedFromThis()));
    }
}

void TestApp::OnShutdown()
{
    _server->Shutdown();
}
