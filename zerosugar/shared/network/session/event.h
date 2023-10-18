#pragma once
#include <variant>
#include "zerosugar/shared/execution/channel/channel.h"
#include "zerosugar/shared/network/buffer/buffer.h"
#include "zerosugar/shared/network/session/id.h"

namespace zerosugar::network::session
{
    struct ReceiveEvent
    {
        id_type id = id_type::Default();
        Buffer buffer;
    };

    struct IoErrorEvent
    {
        id_type id = id_type::Default();
        boost::system::error_code errorCode;
    };

    struct DestructEvent
    {
        id_type id = id_type::Default();
        std::string remoteAddress;
        uint16_t port = 0;
    };

    using event_type = std::variant<ReceiveEvent, IoErrorEvent, DestructEvent>;
    using event_channel_type = execution::Channel<event_type>;
}
