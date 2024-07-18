#pragma once
#include <memory>
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/network/server/server.h"
#include "zerosugar/shared/network/session/session.h"

using zerosugar::SharedPtrNotNull;
using zerosugar::Session;
using zerosugar::session::id_type;

class TestApp;

class TestServer final : public zerosugar::Server
{
public:
    struct Context
    {
        Context(id_type id, SharedPtrNotNull<Session> session)
            : id(id)
            , session(std::move(session))
        {
        }

        id_type id = id_type::Default();
        SharedPtrNotNull<Session> session;
    };

public:
    TestServer(zerosugar::execution::AsioExecutor& executor, TestApp& app);

private:

    void OnAccept(Session& session) override;
    void OnReceive(Session& session, zerosugar::Buffer buffer) override;
    void OnError(Session& session, const boost::system::error_code& error) override;

private:
    TestApp& _app;
};
