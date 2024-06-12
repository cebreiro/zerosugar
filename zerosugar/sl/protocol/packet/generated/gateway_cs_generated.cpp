#include "gateway_cs_generated.h"

#include "zerosugar/sl/protocol/packet/packet_reader.h"
#include "zerosugar/sl/protocol/packet/packet_writer.h"

namespace zerosugar::sl::gateway::cs
{
    void VersionCheckRequest::Deserialize(PacketReader& reader)
    {
        if (reader.Read<int32_t>() != VersionCheckRequest::opcode)
        {
            throw std::runtime_error("VersionCheckRequest invalid opcode");
        }

        version = reader.Read<int32_t>();
    }

    void VersionCheckRequest::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(VersionCheckRequest::opcode);
        writer.Write<int32_t>(version);
    }

    void AuthenticationRequest::Deserialize(PacketReader& reader)
    {
        if (reader.Read<int32_t>() != AuthenticationRequest::opcode)
        {
            throw std::runtime_error("AuthenticationRequest invalid opcode");
        }

        key1 = reader.Read<int32_t>();
        key2 = reader.Read<int32_t>();
        unk01 = reader.Read<UnknownObject>();
    }

    void AuthenticationRequest::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(AuthenticationRequest::opcode);
        writer.Write<int32_t>(key1);
        writer.Write<int32_t>(key2);
        writer.WriteObject(unk01);
    }

    void CharacterListRequest::Deserialize(PacketReader& reader)
    {
        if (reader.Read<int32_t>() != CharacterListRequest::opcode)
        {
            throw std::runtime_error("CharacterListRequest invalid opcode");
        }

    }

    void CharacterListRequest::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(CharacterListRequest::opcode);
    }

    void CharacterSelectRequest::Deserialize(PacketReader& reader)
    {
        if (reader.Read<int32_t>() != CharacterSelectRequest::opcode)
        {
            throw std::runtime_error("CharacterSelectRequest invalid opcode");
        }

        slot = reader.Read<int32_t>();
    }

    void CharacterSelectRequest::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(CharacterSelectRequest::opcode);
        writer.Write<int32_t>(slot);
    }

    void CharacterCreateRequest::Deserialize(PacketReader& reader)
    {
        if (reader.Read<int32_t>() != CharacterCreateRequest::opcode)
        {
            throw std::runtime_error("CharacterCreateRequest invalid opcode");
        }

        create = reader.Read<CharacterCreate>();
        slot = reader.Read<int32_t>();
    }

    void CharacterCreateRequest::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(CharacterCreateRequest::opcode);
        writer.WriteObject(create);
        writer.Write<int32_t>(slot);
    }

    void CharacterDeleteRequest::Deserialize(PacketReader& reader)
    {
        if (reader.Read<int32_t>() != CharacterDeleteRequest::opcode)
        {
            throw std::runtime_error("CharacterDeleteRequest invalid opcode");
        }

        slot = reader.Read<int32_t>();
    }

    void CharacterDeleteRequest::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(CharacterDeleteRequest::opcode);
        writer.Write<int32_t>(slot);
    }

    void CharacterNameCheckRequest::Deserialize(PacketReader& reader)
    {
        if (reader.Read<int32_t>() != CharacterNameCheckRequest::opcode)
        {
            throw std::runtime_error("CharacterNameCheckRequest invalid opcode");
        }

        name = reader.ReadString();
    }

    void CharacterNameCheckRequest::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(CharacterNameCheckRequest::opcode);
        writer.WriteString(name);
    }

    void UnknownRequest0x7040::Deserialize(PacketReader& reader)
    {
        if (reader.Read<int32_t>() != UnknownRequest0x7040::opcode)
        {
            throw std::runtime_error("UnknownRequest0x7040 invalid opcode");
        }

    }

    void UnknownRequest0x7040::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(UnknownRequest0x7040::opcode);
    }

}
