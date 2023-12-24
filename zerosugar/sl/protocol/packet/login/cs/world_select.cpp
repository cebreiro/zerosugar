#include "world_select.h"

#include "zerosugar/sl/protocol/packet/login/cs/opcode.h"

namespace zerosugar::sl::login::cs
{
    WorldSelect::WorldSelect(uint32_t key1, uint32_t key2, int8_t worldId)
        : _key1(key1)
        , _key2(key2)
        , _worldId(worldId)
    {
    }

    auto WorldSelect::Serialize() const -> Buffer
    {
        Buffer buffer;
        buffer.Add(buffer::Fragment::Create(fixed_buffer_size));

        BufferWriter writer(buffer);
        writer.Write(_key1);
        writer.Write(_key2);
        writer.Write(_worldId);

        return buffer;
    }

    auto WorldSelect::Deserialize(const Buffer& buffer) -> LoginPacketDeserializeResult
    {
        if (buffer.GetSize() < fixed_buffer_size)
        {
            return LoginPacketDeserializeResult{
                .errorCode = LoginPacketHandlerErrorCode::Fail_ShortLength
            };
        }

        BufferReader reader(buffer.cbegin(), buffer.cend());
        _key1 = reader.Read<uint32_t>();
        _key2 = reader.Read<uint32_t>();
        _worldId = reader.Read<int8_t>();

        assert(reader.GetReadSize() == fixed_buffer_size);

        return LoginPacketDeserializeResult{
            .errorCode = LoginPacketHandlerErrorCode::None,
            .readSize = reader.GetReadSize(),
        };
    }

    auto WorldSelect::GetOpcode() const -> int8_t
    {
        return static_cast<int8_t>(Opcode::WorldSelectRequest);
    }

    auto WorldSelect::GetKey1() const -> uint32_t
    {
        return _key1;
    }

    auto WorldSelect::GetKey2() const -> uint32_t
    {
        return _key2;
    }

    auto WorldSelect::GetWorldId() const -> int8_t
    {
        return _worldId;
    }
}
