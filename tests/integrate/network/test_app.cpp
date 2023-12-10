#include "test_app.h"

#include "tests/integrate/network/test_server.h"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"

TestApp::TestApp()
    : _executor(std::make_shared<AsioExecutor>(static_cast<int64_t>(std::thread::hardware_concurrency())))
    , _server(std::make_shared<TestServer>(*_executor, *this))
{
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

    if (!_server->StartUp(PORT))
    {
        throw std::runtime_error("fail to start server");
    }

    for (int64_t i = 0; i < static_cast<int64_t>(std::thread::hardware_concurrency()); ++i)
    {
        _strands.emplace_back(std::make_shared<Strand>(_executor->SharedFromThis()));
    }
}

void TestApp::OnShutdown()
{
    _server->Shutdown();
}
