#pragma once
#include "zerosugar/sl/protocol/packet/login/login_packet_concept.h"

namespace zerosugar::sl::login::sc
{
    enum class LoginFailReason : int32_t
    {
        CantConnectServer = 1,
        InvalidIDPassword = 2,
        ReleaseExistingConnection = 7,
        AgeRestriction = 12,
        TooMuchUser = 15,
        NotPaidAccount = 18,
    };

    class LoginFail
    {
    public:
        LoginFail() = default;
        explicit LoginFail(LoginFailReason reason);

        auto Serialize() const -> Buffer;
        auto Deserialize(const Buffer& buffer) -> LoginPacketDeserializeResult;

        auto GetOpcode() const -> int8_t;

    private:
        LoginFailReason _reason;

        static constexpr int64_t fixed_buffer_size = sizeof(_reason);
    };
}
