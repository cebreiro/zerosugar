#pragma once

namespace zerosugar::sl
{
    enum class LoginPacketDeserializeErrorCode
    {
        None = 0,
        Fail_InvalidFormat,
        Fail_ShortLength,
    };

    struct LoginPacketDeserializeResult
    {
        LoginPacketDeserializeErrorCode errorCode = LoginPacketDeserializeErrorCode::None;
        int64_t readSize = 0;
    };

    template <typename T>
    concept login_packet_concept = requires (T t, const Buffer& buffer)
    {
        requires std::default_initializable<T>;
        { static_cast<const T&>(t).Serialize() } -> std::same_as<Buffer>;
        { t.Deserialize(buffer) } -> std::same_as<LoginPacketDeserializeResult>;
        { static_cast<const T&>(t).GetOpcode() } -> std::same_as<int8_t>;
    };
}
