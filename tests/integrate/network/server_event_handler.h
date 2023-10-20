#pragma once
#include <memory>
#include <tbb/concurrent_hash_map.h>
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/network/server/event.h"
#include "zerosugar/shared/network/session/session.h"

using zerosugar::SharedPtrNotNull;
using zerosugar::Future;
using zerosugar::Session;
using zerosugar::session::id_type;
using zerosugar::server::ConnectionEvent;
using zerosugar::server::DisconnectionEvent;
using zerosugar::server::ReceiveEvent;

class TestApp;

class ServerEventHandler : public std::enable_shared_from_this<ServerEventHandler>
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
    explicit ServerEventHandler(TestApp& app);

    auto Run() -> Future<bool>;

private:
    auto RunImpl() -> Future<bool>;

    bool HandleEvent(ConnectionEvent event);
    bool HandleEvent(DisconnectionEvent event);
    bool HandleEvent(ReceiveEvent event);

private:
    TestApp& _app;
    tbb::concurrent_hash_map<id_type, std::shared_ptr<Context>> _table;
};
