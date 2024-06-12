#include "login_session_state_machine.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/model/generated/login_cs_generated.h"

namespace zerosugar::xr
{
    LoginServerSessionStateMachine::LoginServerSessionStateMachine(Session& session)
    {
        AddState<login_session::ConnectState>(LoginSessionState::Connected, true)
            .Add(LoginSessionState::Authenticated);

        AddState<login_session::AuthenticatedState>(LoginSessionState::Authenticated, false)
            .Add(LoginSessionState::TransitionToLobby);

        AddState<login_session::AuthenticatedState>(LoginSessionState::TransitionToLobby, false, session);
    }
}
