#include "test_app.h"

#include "tests/integrate/network/test_server.h"

TestApp::TestApp()
    : _server(std::make_shared<TestServer>(GetAsioExecutor(), *this))
{
}

auto TestApp::GetServer() -> TestServer&
{
    return *_server;
}

auto TestApp::GetStrands() const -> const std::vector<SharedPtrNotNull<Strand>>&
{
    return _strands;
}

void TestApp::OnStartUp(std::span<char*> args)
{
    (void)args;

    if (!_server->StartUp(PORT))
    {
        throw std::runtime_error("fail to start server");
    }

    for (int64_t i = 0; i < static_cast<int64_t>(std::thread::hardware_concurrency()); ++i)
    {
        _strands.emplace_back(std::make_shared<Strand>(GetExecutor().SharedFromThis()));
    }
}

void TestApp::OnShutdown()
{
    _server->Shutdown();
}
