#include "world_enter.h"

#include "zerosugar/sl/protocol/packet/login/sc/opcode.h"

namespace zerosugar::sl::login::sc
{
    WorldEnter::WorldEnter(uint32_t key1, uint32_t key2, int8_t worldId)
        : _key1(key1)
        , _key2(key2)
        , _worldId(worldId)
    {
    }

    auto WorldEnter::Serialize() const -> Buffer
    {
        Buffer buffer;
        buffer.Add(buffer::Fragment::Create(fixed_buffer_size));

        BufferWriter writer(buffer);
        writer.Write(_key1);
        writer.Write(_key2);
        writer.Write(_worldId);

        return buffer;
    }

    auto WorldEnter::Deserialize(const Buffer& buffer) -> LoginPacketDeserializeResult
    {
        if (buffer.GetSize() < fixed_buffer_size)
        {
            return LoginPacketDeserializeResult{
                .errorCode = LoginPacketDeserializeResult::ErrorCode::Fail_ShortLength
            };
        }

        BufferReader reader(buffer.cbegin(), buffer.cend());
        _key1 = reader.Read<uint32_t>();
        _key2 = reader.Read<uint32_t>();
        _worldId = reader.Read<int8_t>();

        assert(reader.GetReadSize() == fixed_buffer_size);

        return LoginPacketDeserializeResult{
            .errorCode = LoginPacketDeserializeResult::ErrorCode::None,
            .readSize = reader.GetReadSize(),
        };
    }

    auto WorldEnter::GetOpcode() const -> int8_t
    {
        return static_cast<int8_t>(Opcode::WorldEnter);
    }

    auto WorldEnter::GetKey1() const -> uint32_t
    {
        return _key1;
    }

    auto WorldEnter::GetKey2() const -> uint32_t
    {
        return _key2;
    }

    auto WorldEnter::GetWorldId() const -> int8_t
    {
        return _worldId;
    }
}
