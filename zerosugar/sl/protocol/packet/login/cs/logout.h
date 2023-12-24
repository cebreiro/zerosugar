#pragma once
#include "zerosugar/sl/protocol/packet/login/login_packet_concept.h"

namespace zerosugar::sl::login::cs
{
    class Logout
    {
    public:
        Logout() = default;

        auto Serialize() const -> Buffer;
        auto Deserialize(const Buffer& buffer) -> LoginPacketDeserializeResult;

        auto GetOpcode() const -> int8_t;
    };
}
