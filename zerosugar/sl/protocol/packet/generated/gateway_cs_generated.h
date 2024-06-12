#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "zerosugar/sl/protocol/packet/packet_deserializable.h"
#include "zerosugar/sl/protocol/packet/packet_serializable.h"
#include "zerosugar/sl/protocol/packet/generated/gateway_object_generated.h"

namespace zerosugar::sl::gateway::cs
{
    struct VersionCheckRequest : IPacketDeserializable, IPacketSerializable
    {
        static constexpr int32_t opcode = 28672;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;

        int32_t version = {};
    };

    struct AuthenticationRequest : IPacketDeserializable, IPacketSerializable
    {
        static constexpr int32_t opcode = 28674;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;

        int32_t key1 = {};
        int32_t key2 = {};
        UnknownObject unk01 = {};
    };

    struct CharacterListRequest : IPacketDeserializable, IPacketSerializable
    {
        static constexpr int32_t opcode = 28676;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
    };

    struct CharacterSelectRequest : IPacketDeserializable, IPacketSerializable
    {
        static constexpr int32_t opcode = 28677;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;

        int32_t slot = {};
    };

    struct CharacterCreateRequest : IPacketDeserializable, IPacketSerializable
    {
        static constexpr int32_t opcode = 28679;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;

        CharacterCreate create = {};
        int32_t slot = {};
    };

    struct CharacterDeleteRequest : IPacketDeserializable, IPacketSerializable
    {
        static constexpr int32_t opcode = 28680;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;

        int32_t slot = {};
    };

    struct CharacterNameCheckRequest : IPacketDeserializable, IPacketSerializable
    {
        static constexpr int32_t opcode = 28682;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;

        std::string name = {};
    };

    struct UnknownRequest0x7040 : IPacketDeserializable, IPacketSerializable
    {
        static constexpr int32_t opcode = 28736;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
    };

}
