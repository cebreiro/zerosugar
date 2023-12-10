#pragma once
#include <cstdint>
#include <concepts>
#include "zerosugar/shared/network/buffer/buffer.h"

namespace zerosugar::sl
{
    struct LoginPacketDeserializeResult
    {
        enum class ErrorCode
        {
            None = 0,
            Fail_InvalidFormat,
            Fail_ShortLength,
        };

        ErrorCode errorCode = ErrorCode::None;
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
