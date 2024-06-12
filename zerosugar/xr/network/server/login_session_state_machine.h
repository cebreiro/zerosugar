#pragma once
#include "zerosugar/shared/state_machine/state_machine.h"
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/network/server/login_session_state.h"

namespace zerosugar
{
    class Session;
}

namespace zerosugar::xr
{
    class LoginServerSessionStateMachine : public StateMachine<LoginSessionState, StateEvent<IPacket, Future<void>>>
    {
    public:
        explicit LoginServerSessionStateMachine(Session& session);
    };
}
