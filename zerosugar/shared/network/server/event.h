#pragma once
#include <variant>
#include <boost/asio.hpp>
#include "zerosugar/shared/execution/channel/channel.h"
#include "zerosugar/shared/network/buffer/buffer.h"
#include "zerosugar/shared/network/session/id.h"

namespace zerosugar::server
{
    struct AcceptEvent
    {
        boost::asio::ip::tcp::socket socket;
    };

    struct AcceptError
    {
        boost::system::error_code errorCode;
    };

    struct SessionReceiveEvent
    {
        session::id_type id = session::id_type::Default();
        Buffer buffer;
    };

    struct SessionIoErrorEvent
    {
        session::id_type id = session::id_type::Default();
        boost::system::error_code errorCode;
    };

    struct SessionDestructEvent
    {
        session::id_type id = session::id_type::Default();
    };

    using event_type = std::variant<std::monostate, AcceptEvent, AcceptError, SessionReceiveEvent, SessionIoErrorEvent, SessionDestructEvent>;
    using event_channel_type = Channel<event_type>;
}
