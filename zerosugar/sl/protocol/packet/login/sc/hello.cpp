#include "hello.h"

#include "zerosugar/sl/protocol/packet/login/sc/opcode.h"

namespace zerosugar::sl::login::sc
{
    Hello::Hello(uint32_t key1, uint32_t key2)
        : _key1(key1)
        , _key2(key2)
    {
    }

    auto Hello::Serialize() const -> Buffer
    {
        Buffer buffer;
        buffer.Add(buffer::Fragment::Create(sizeof(_key1) + sizeof(_key2)));

        BufferWriter writer(buffer);
        writer.Write(_key1);
        writer.Write(_key2);

        return buffer;
    }

    auto Hello::Deserialize(const Buffer& buffer) -> LoginPacketDeserializeResult
    {
        if (buffer.GetSize() < 8)
        {
            return LoginPacketDeserializeResult{
                .errorCode = LoginPacketDeserializeErrorCode::Fail_ShortLength
            };
        }

        BufferReader reader(buffer.cbegin(), buffer.cend());
        _key1 = reader.Read<uint32_t>();
        _key2 = reader.Read<uint32_t>();

        return LoginPacketDeserializeResult{
            .errorCode = LoginPacketDeserializeErrorCode::None,
            .readSize = reader.GetReadSize(),
        };
    }

    auto Hello::GetOpcode() const -> int8_t
    {
        return static_cast<int8_t>(Opcode::Hello);
    }
}
