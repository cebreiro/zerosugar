#pragma once
#include "zerosugar/sl/protocol/packet/login/login_packet_concept.h"

namespace zerosugar::sl::login::sc
{
    class WorldEnter
    {
    public:
        WorldEnter() = default;
        WorldEnter(uint32_t key1, uint32_t key2, int8_t worldId);

        auto Serialize() const -> Buffer;
        auto Deserialize(const Buffer& buffer) -> LoginPacketDeserializeResult;

        auto GetOpcode() const -> int8_t;

        auto GetKey1() const -> uint32_t;
        auto GetKey2() const -> uint32_t;
        auto GetWorldId() const -> int8_t;

    private:
        uint32_t _key1 = 0;
        uint32_t _key2 = 0;
        uint8_t _worldId = 0;

        static constexpr int64_t fixed_buffer_size = 4 + 4 + 1;
    };
}
