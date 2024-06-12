#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "zerosugar/sl/protocol/packet/packet_deserializable.h"
#include "zerosugar/sl/protocol/packet/packet_serializable.h"
#include "zerosugar/sl/protocol/packet/generated/gateway_object_generated.h"

namespace zerosugar::sl::gateway::sc
{
    struct VersionCheckResponse : IPacketDeserializable, IPacketSerializable
    {
        static constexpr int32_t opcode = 32768;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;

        int32_t fail = {};
    };

    struct AuthenticationResponse : IPacketDeserializable, IPacketSerializable
    {
        static constexpr int32_t opcode = 32770;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;

        int32_t fail = {};
        std::string unkEssentialString = {};
    };

    struct CharacterListResponse : IPacketDeserializable, IPacketSerializable
    {
        static constexpr int32_t opcode = 32771;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;

        int32_t unk01 = {};
        int32_t chractersCount = {};
        int32_t unk02 = {};
        std::vector<Character> characters = {};
    };

    struct CharacterSelectResponse : IPacketDeserializable, IPacketSerializable
    {
        static constexpr int32_t opcode = 32772;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;

        int32_t fail = {};
        int32_t auth = {};
        std::string token = {};
        EndPoint zoneEndPoint = {};
    };

    struct CharacterCreateResponse : IPacketDeserializable, IPacketSerializable
    {
        static constexpr int32_t opcode = 32774;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;

        int32_t fail = {};
    };

    struct CharacterDeleteResponse : IPacketDeserializable, IPacketSerializable
    {
        static constexpr int32_t opcode = 32775;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;

        int32_t fail = {};
    };

    struct CharacterNameCheckResponse : IPacketDeserializable, IPacketSerializable
    {
        static constexpr int32_t opcode = 32785;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;

        int32_t fail = {};
        std::string name = {};
    };

}
