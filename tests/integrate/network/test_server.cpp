#include "test_server.h"

#include "tests/integrate/network/test_app.h"
#include "zerosugar/shared/execution/executor/operation/dispatch.h"


TestServer::TestServer(zerosugar::execution::AsioExecutor& executor, TestApp& app)
    : Server("integrate_test", app.GetServiceLocator(), executor)
    , _app(app)
{
}

void TestServer::OnAccept(Session& session)
{
    zerosugar::session::id_type sessionId = session.GetId();
    auto client = std::make_shared<Context>(sessionId, session.shared_from_this());

    decltype(_table)::accessor accessor;
    if (_table.insert(accessor, sessionId))
    {
        accessor->second = std::move(client);
    }
}

void TestServer::OnReceive(Session& session, zerosugar::Buffer buffer)
{
    std::shared_ptr<Context> context;

    decltype(_table)::const_accessor accessor;
    if (_table.find(accessor, session.GetId()))
    {
        context = accessor->second;
        assert(context);
    }
    else
    {
        return;
    }

    const auto& strands = _app.GetStrands();
    auto& strand = *strands[session.GetId().Unwrap() % strands.size()];

    Dispatch(strand, [buffer = std::move(buffer), context]() mutable
        {
            context->session->Send(std::move(buffer));
        });
}

void TestServer::OnError(Session& session, const boost::system::error_code& error)
{
    (void)error;

    _table.erase(session.GetId());
}
