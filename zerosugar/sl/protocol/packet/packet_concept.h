#pragma once

namespace zerosugar::sl
{
    enum class PacketDeserializeErrorCode
    {
        None = 0,
        Fail_InvalidFormat,
        Fail_ShortLength,
    };

    struct PacketDeserializeResult
    {
        PacketDeserializeErrorCode errorCode = PacketDeserializeErrorCode::None;
        int64_t readSize = 0;
    };

    template <typename T>
    concept packet_concept = requires (T t, const Buffer& buffer)
    {
        requires std::default_initializable<T>;
        { static_cast<const T&>(t).Serialize() } -> std::same_as<Buffer>;
        { t.Deserialize(buffer) } -> std::same_as<PacketDeserializeResult>;
        { static_cast<const T&>(t).GetOpcode() } -> std::same_as<int32_t>;
    };
}
