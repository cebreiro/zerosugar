#include "test_server.h"

#include "tests/integrate/network/test_app.h"
#include "zerosugar/shared/execution/executor/operation/dispatch.h"


TestServer::TestServer(zerosugar::execution::AsioExecutor& executor, TestApp& app)
    : Server("integrate_test", executor)
    , _app(app)
{
    (void)_app;
}

void TestServer::OnAccept(Session& session)
{
    (void)session;
}

void TestServer::OnReceive(Session& session, zerosugar::Buffer buffer)
{
    session.Send(std::move(buffer));
}

void TestServer::OnError(Session& session, const boost::system::error_code& error)
{
    (void)session;
    (void)error;
}
