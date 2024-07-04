#include "lobby_sc_message.h"

#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr::network::lobby::sc
{
    void FailAuthenticate::Deserialize(PacketReader& reader)
    {
        (void)reader;
    }

    void FailAuthenticate::Serialize(PacketWriter& writer) const
    {
        (void)writer;
    }

    void ResultCreateCharacter::Deserialize(PacketReader& reader)
    {
        success = reader.Read<bool>();
        character = reader.Read<LobbyCharacter>();
    }

    void ResultCreateCharacter::Serialize(PacketWriter& writer) const
    {
        writer.Write<bool>(success);
        writer.Write(character);
    }

    void SuccessDeleteCharacter::Deserialize(PacketReader& reader)
    {
        slot = reader.Read<int32_t>();
    }

    void SuccessDeleteCharacter::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(slot);
    }

    void NotifyCharacterList::Deserialize(PacketReader& reader)
    {
        count = reader.Read<int32_t>();
        for (int32_t i = 0; i < count; ++i)
        {
            character.emplace_back(reader.Read<LobbyCharacter>());
        }
    }

    void NotifyCharacterList::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(count);
        for (const auto& item : character)
        {
            writer.WriteObject(item);
        }
    }

    void SuccessSelectCharacter::Deserialize(PacketReader& reader)
    {
        ip = reader.ReadString();
        port = reader.Read<int32_t>();
    }

    void SuccessSelectCharacter::Serialize(PacketWriter& writer) const
    {
        writer.Write(ip);
        writer.Write<int32_t>(port);
    }

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>
    {
        const int16_t opcode = reader.Read<int16_t>();
        switch(opcode)
        {
            case FailAuthenticate::opcode:
            {
                auto item = std::make_unique<FailAuthenticate>();
                item->Deserialize(reader);

                return item;
            }
            case ResultCreateCharacter::opcode:
            {
                auto item = std::make_unique<ResultCreateCharacter>();
                item->Deserialize(reader);

                return item;
            }
            case SuccessDeleteCharacter::opcode:
            {
                auto item = std::make_unique<SuccessDeleteCharacter>();
                item->Deserialize(reader);

                return item;
            }
            case NotifyCharacterList::opcode:
            {
                auto item = std::make_unique<NotifyCharacterList>();
                item->Deserialize(reader);

                return item;
            }
            case SuccessSelectCharacter::opcode:
            {
                auto item = std::make_unique<SuccessSelectCharacter>();
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
            case FailAuthenticate::opcode:
            {
                FailAuthenticate item;
                item.Deserialize(reader);

                return item;
            }
            case ResultCreateCharacter::opcode:
            {
                ResultCreateCharacter item;
                item.Deserialize(reader);

                return item;
            }
            case SuccessDeleteCharacter::opcode:
            {
                SuccessDeleteCharacter item;
                item.Deserialize(reader);

                return item;
            }
            case NotifyCharacterList::opcode:
            {
                NotifyCharacterList item;
                item.Deserialize(reader);

                return item;
            }
            case SuccessSelectCharacter::opcode:
            {
                SuccessSelectCharacter item;
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
            case FailAuthenticate::opcode:
            {
                return typeid(FailAuthenticate);
            }
            case ResultCreateCharacter::opcode:
            {
                return typeid(ResultCreateCharacter);
            }
            case SuccessDeleteCharacter::opcode:
            {
                return typeid(SuccessDeleteCharacter);
            }
            case NotifyCharacterList::opcode:
            {
                return typeid(NotifyCharacterList);
            }
            case SuccessSelectCharacter::opcode:
            {
                return typeid(SuccessSelectCharacter);
            }
        }
        assert(false);
        return typeid(nullptr);
    }
}
