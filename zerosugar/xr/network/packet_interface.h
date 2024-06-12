#pragma once
#include "zerosugar/xr/network/packet_deserializable.h"
#include "zerosugar/xr/network/packet_serializable.h"

namespace zerosugar::xr
{
    class IPacket : public IPacketDeserializable, public IPacketSerializable
    {
    public:
        virtual ~IPacket() = default;

        template <typename T> requires std::derived_from<T, IPacket>
        auto Cast() const -> const T*
        {
            if (this->GetOpcode() == T::opcode)
            {
                const T* ptr = static_cast<const T*>(this);
                assert(ptr == dynamic_cast<const T*>(this));

                return ptr;
            }

            return nullptr;
        }

        virtual auto GetOpcode() const -> int32_t = 0;
    };
}
