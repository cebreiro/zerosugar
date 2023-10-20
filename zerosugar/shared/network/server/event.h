#pragma once
#include <variant>
#include <boost/asio.hpp>
#include "zerosugar/shared/execution/channel/channel.h"
#include "zerosugar/shared/network/buffer/buffer.h"
#include "zerosugar/shared/network/session/id.h"

namespace zerosugar
{
    class Session;
}

namespace zerosugar::server
{
    struct ConnectionEvent
    {
        session::id_type id = session::id_type::Default();
        SharedPtrNotNull<Session> session;
    };

    struct ReceiveEvent
    {
        session::id_type id = session::id_type::Default();
        Buffer buffer;
    };

    struct DisconnectionEvent
    {
        session::id_type id = session::id_type::Default();
    };

    using event_type = std::variant<ConnectionEvent, ReceiveEvent, DisconnectionEvent>;
    using event_channel_type = Channel<event_type>;
}
