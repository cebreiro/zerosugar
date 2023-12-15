#include "world_list.h"

#include <WinSock2.h>
#include <ws2tcpip.h>

#include "zerosugar/sl/protocol/packet/login/sc/opcode.h"

namespace
{
    auto ConvertAddress(const std::string& address) -> int32_t
    {
        SOCKADDR_IN sa = {};
        if (!::inet_pton(AF_INET, address.data(), &sa.sin_addr.s_addr))
        {
            return -1;
        }

        return static_cast<int32_t>(sa.sin_addr.s_addr);
    }
}

namespace zerosugar::sl::login::sc
{
    auto World::Construct(int8_t id, const std::string& src, const std::string& dest) -> World
    {
        return World{
            .id = id,
            .src = ConvertAddress(src),
            .dest = ConvertAddress(dest),
        };
    }

    WorldList::WorldList(std::span<const World> worlds)
        : _worlds(worlds.begin(), worlds.end())
    {
    }

    auto WorldList::Serialize() const -> Buffer
    {
        const int64_t worldCount = std::ssize(_worlds);
        if (worldCount <= 0)
        {
            return {};
        }

        const int64_t size = 1 + (worldCount * (1 + 4 + 4));

        Buffer buffer;
        buffer.Add(buffer::Fragment::Create(size));

        BufferWriter writer(buffer);
        writer.Write<int8_t>(static_cast<int8_t>(worldCount));

        for (int8_t i = 0; i < worldCount; ++i)
        {
            const World& world = _worlds[i];

            writer.Write<int8_t>(world.id);
            writer.Write<int32_t>(world.src);
            writer.Write<int32_t>(world.dest);
        }

        return buffer;
    }

    auto WorldList::Deserialize(const Buffer& buffer) -> LoginPacketDeserializeResult
    {
        if (buffer.GetSize() < 1)
        {
            return LoginPacketDeserializeResult{
                .errorCode = LoginPacketDeserializeResult::ErrorCode::Fail_ShortLength
            };
        }

        BufferReader reader(buffer.cbegin(), buffer.cend());

        const int64_t worldCount = reader.Read<int8_t>();
        if (worldCount <= 0)
        {
            return LoginPacketDeserializeResult{
                .errorCode = LoginPacketDeserializeResult::ErrorCode::Fail_InvalidFormat
            };
        }

        if (buffer.GetSize() < 1 + (worldCount * (1 + 4 + 4)))
        {
            return LoginPacketDeserializeResult{
                .errorCode = LoginPacketDeserializeResult::ErrorCode::Fail_ShortLength
            };
        }

        for (int64_t i = 0; i < worldCount; ++i)
        {
            const int8_t id = reader.Read<int8_t>();
            const int32_t src = reader.Read<int32_t>();
            const int32_t dest = reader.Read<int32_t>();

            _worlds.emplace_back(World{
                .id = id,
                .src = src,
                .dest = dest,
                });
        }

        return LoginPacketDeserializeResult{
            .errorCode = LoginPacketDeserializeResult::ErrorCode::None,
            .readSize = reader.GetReadSize(),
        };
    }

    auto WorldList::GetOpcode() const -> int8_t
    {
        return static_cast<int8_t>(Opcode::WorldList);
    }
}
