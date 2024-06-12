#include "gateway_sc_generated.h"

#include "zerosugar/sl/protocol/packet/packet_reader.h"
#include "zerosugar/sl/protocol/packet/packet_writer.h"

namespace zerosugar::sl::gateway::sc
{
    void VersionCheckResponse::Deserialize(PacketReader& reader)
    {
        if (reader.Read<int32_t>() != VersionCheckResponse::opcode)
        {
            throw std::runtime_error("VersionCheckResponse invalid opcode");
        }

        fail = reader.Read<int32_t>();
    }

    void VersionCheckResponse::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(VersionCheckResponse::opcode);
        writer.Write<int32_t>(fail);
    }

    void AuthenticationResponse::Deserialize(PacketReader& reader)
    {
        if (reader.Read<int32_t>() != AuthenticationResponse::opcode)
        {
            throw std::runtime_error("AuthenticationResponse invalid opcode");
        }

        fail = reader.Read<int32_t>();
        unkEssentialString = reader.ReadString();
    }

    void AuthenticationResponse::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(AuthenticationResponse::opcode);
        writer.Write<int32_t>(fail);
        writer.WriteString(unkEssentialString);
    }

    void CharacterListResponse::Deserialize(PacketReader& reader)
    {
        if (reader.Read<int32_t>() != CharacterListResponse::opcode)
        {
            throw std::runtime_error("CharacterListResponse invalid opcode");
        }

        unk01 = reader.Read<int32_t>();
        chractersCount = reader.Read<int32_t>();
        unk02 = reader.Read<int32_t>();
        for (int32_t i = 0; i < chractersCount; ++i)
        {
            characters.emplace_back(reader.Read<Character>());
        }
    }

    void CharacterListResponse::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(CharacterListResponse::opcode);
        writer.Write<int32_t>(unk01);
        writer.Write<int32_t>(chractersCount);
        writer.Write<int32_t>(unk02);
        for (const auto& item : characters)
        {
            writer.WriteObject(item);
        }
    }

    void CharacterSelectResponse::Deserialize(PacketReader& reader)
    {
        if (reader.Read<int32_t>() != CharacterSelectResponse::opcode)
        {
            throw std::runtime_error("CharacterSelectResponse invalid opcode");
        }

        fail = reader.Read<int32_t>();
        auth = reader.Read<int32_t>();
        token = reader.ReadString();
        zoneEndPoint = reader.Read<EndPoint>();
    }

    void CharacterSelectResponse::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(CharacterSelectResponse::opcode);
        writer.Write<int32_t>(fail);
        writer.Write<int32_t>(auth);
        writer.WriteString(token);
        writer.WriteObject(zoneEndPoint);
    }

    void CharacterCreateResponse::Deserialize(PacketReader& reader)
    {
        if (reader.Read<int32_t>() != CharacterCreateResponse::opcode)
        {
            throw std::runtime_error("CharacterCreateResponse invalid opcode");
        }

        fail = reader.Read<int32_t>();
    }

    void CharacterCreateResponse::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(CharacterCreateResponse::opcode);
        writer.Write<int32_t>(fail);
    }

    void CharacterDeleteResponse::Deserialize(PacketReader& reader)
    {
        if (reader.Read<int32_t>() != CharacterDeleteResponse::opcode)
        {
            throw std::runtime_error("CharacterDeleteResponse invalid opcode");
        }

        fail = reader.Read<int32_t>();
    }

    void CharacterDeleteResponse::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(CharacterDeleteResponse::opcode);
        writer.Write<int32_t>(fail);
    }

    void CharacterNameCheckResponse::Deserialize(PacketReader& reader)
    {
        if (reader.Read<int32_t>() != CharacterNameCheckResponse::opcode)
        {
            throw std::runtime_error("CharacterNameCheckResponse invalid opcode");
        }

        fail = reader.Read<int32_t>();
        name = reader.ReadString();
    }

    void CharacterNameCheckResponse::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(CharacterNameCheckResponse::opcode);
        writer.Write<int32_t>(fail);
        writer.WriteString(name);
    }

}
