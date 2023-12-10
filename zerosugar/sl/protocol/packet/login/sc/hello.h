#pragma once
#include <cstdint>
#include "zerosugar/sl/protocol/packet/login/login_packet_concept.h"

namespace zerosugar::sl::login::sc
{
    class Hello
    {
    public:
        Hello() = default;
        Hello(uint32_t key1, uint32_t key2);

        auto Serialize() const -> Buffer;
        auto Deserialize(const Buffer& buffer) -> LoginPacketDeserializeResult;

        auto GetOpcode() const -> int8_t;

    private:
        uint32_t _key1 = 0;
        uint32_t _key2 = 0;
    };
}
