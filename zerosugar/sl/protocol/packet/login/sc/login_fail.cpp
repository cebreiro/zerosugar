#include "login_fail.h"

#include "zerosugar/sl/protocol/packet/login/sc/opcode.h"

namespace zerosugar::sl::login::sc
{
    LoginFail::LoginFail(LoginFailReason reason)
        : _reason(reason)
    {
    }

    auto LoginFail::Serialize() const -> Buffer
    {
        Buffer buffer;
        buffer.Add(buffer::Fragment::Create(fixed_buffer_size));

        BufferWriter writer(buffer);
        writer.Write<int32_t>(static_cast<int32_t>(_reason));

        return buffer;
    }

    auto LoginFail::Deserialize(const Buffer& buffer) -> LoginPacketDeserializeResult
    {
        if (buffer.GetSize() < fixed_buffer_size)
        {
            return LoginPacketDeserializeResult{
                .errorCode = LoginPacketHandlerErrorCode::Fail_ShortLength
            };
        }

        BufferReader reader(buffer.cbegin(), buffer.cend());
        _reason = static_cast<LoginFailReason>(reader.Read<int32_t>());

        assert(reader.GetReadSize() == fixed_buffer_size);

        return LoginPacketDeserializeResult{
            .errorCode = LoginPacketHandlerErrorCode::None,
            .readSize = reader.GetReadSize(),
        };
    }

    auto LoginFail::GetOpcode() const -> int8_t
    {
        return static_cast<int8_t>(Opcode::LoginFail);
    }
}
