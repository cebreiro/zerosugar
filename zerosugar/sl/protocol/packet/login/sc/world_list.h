#pragma once
#include <cstdint>
#include "zerosugar/sl/protocol/packet/login/login_packet_concept.h"

namespace zerosugar::sl::login::sc
{
    struct World
    {
        int8_t id = 0;
        int32_t src = 0;
        int32_t dest = 0;

        static auto Construct(int8_t id, const std::string& src, const std::string& dest) -> World;
    };

    class WorldList
    {
    public:
        WorldList() = default;
        explicit WorldList(std::span<const World> worlds);

        auto Serialize() const -> Buffer;
        auto Deserialize(const Buffer& buffer) -> LoginPacketDeserializeResult;

        auto GetOpcode() const -> int8_t;

    private:
        std::vector<World> _worlds;
    };
}
