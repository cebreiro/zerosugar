#include "logout.h"

#include "zerosugar/sl/protocol/packet/login/cs/opcode.h"

namespace zerosugar::sl::login::cs
{
    auto Logout::Serialize() const -> Buffer
    {
        return {};
    }

    auto Logout::Deserialize(const Buffer& buffer) -> LoginPacketDeserializeResult
    {
        (void)buffer;

        return LoginPacketDeserializeResult{
            .errorCode = LoginPacketDeserializeErrorCode::None,
            .readSize = 0,
        };
    }

    auto Logout::GetOpcode() const -> int8_t
    {
        return static_cast<int8_t>(Opcode::LogoutRequest);
    }
}
