#include "lobby_cs_message.h"

#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr::network::lobby::cs
{
    void Authenticate::Deserialize(PacketReader& reader)
    {
        authenticationToken = reader.ReadString();
    }

    void Authenticate::Serialize(PacketWriter& writer) const
    {
        writer.Write(authenticationToken);
    }

    void CreateCharacter::Deserialize(PacketReader& reader)
    {
        character = reader.Read<LobbyCharacter>();
    }

    void CreateCharacter::Serialize(PacketWriter& writer) const
    {
        writer.Write(character);
    }

    void DeleteCharacter::Deserialize(PacketReader& reader)
    {
        slot = reader.Read<int32_t>();
    }

    void DeleteCharacter::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(slot);
    }

    void SelectCharacter::Deserialize(PacketReader& reader)
    {
        slot = reader.Read<int32_t>();
    }

    void SelectCharacter::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(slot);
    }

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>
    {
        const int16_t opcode = reader.Read<int16_t>();
        switch(opcode)
        {
            case Authenticate::opcode:
            {
                auto item = std::make_unique<Authenticate>();
                item->Deserialize(reader);

                return item;
            }
            case CreateCharacter::opcode:
            {
                auto item = std::make_unique<CreateCharacter>();
                item->Deserialize(reader);

                return item;
            }
            case DeleteCharacter::opcode:
            {
                auto item = std::make_unique<DeleteCharacter>();
                item->Deserialize(reader);

                return item;
            }
            case SelectCharacter::opcode:
            {
                auto item = std::make_unique<SelectCharacter>();
                item->Deserialize(reader);

                return item;
            }
        }
        return {};
    }

    auto CreateAnyFrom(PacketReader& reader) -> std::any
    {
        const int16_t opcode = reader.Read<int16_t>();
        switch(opcode)
        {
            case Authenticate::opcode:
            {
                Authenticate item;
                item.Deserialize(reader);

                return item;
            }
            case CreateCharacter::opcode:
            {
                CreateCharacter item;
                item.Deserialize(reader);

                return item;
            }
            case DeleteCharacter::opcode:
            {
                DeleteCharacter item;
                item.Deserialize(reader);

                return item;
            }
            case SelectCharacter::opcode:
            {
                SelectCharacter item;
                item.Deserialize(reader);

                return item;
            }
        }
        return {};
    }

    auto GetPacketTypeInfo(int32_t opcode) -> const std::type_info&
    {
        switch(opcode)
        {
            case Authenticate::opcode:
            {
                return typeid(Authenticate);
            }
            case CreateCharacter::opcode:
            {
                return typeid(CreateCharacter);
            }
            case DeleteCharacter::opcode:
            {
                return typeid(DeleteCharacter);
            }
            case SelectCharacter::opcode:
            {
                return typeid(SelectCharacter);
            }
        }
        assert(false);
        return typeid(nullptr);
    }
}
