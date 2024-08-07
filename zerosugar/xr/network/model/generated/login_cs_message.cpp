#include "login_cs_message.h"

#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr::network::login::cs
{
    void CreateAccount::Deserialize(PacketReader& reader)
    {
        account = reader.ReadString();
        password = reader.ReadString();
    }

    void CreateAccount::Serialize(PacketWriter& writer) const
    {
        writer.Write(account);
        writer.Write(password);
    }

    void Login::Deserialize(PacketReader& reader)
    {
        account = reader.ReadString();
        password = reader.ReadString();
    }

    void Login::Serialize(PacketWriter& writer) const
    {
        writer.Write(account);
        writer.Write(password);
    }

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>
    {
        const int16_t opcode = reader.Read<int16_t>();
        switch(opcode)
        {
            case CreateAccount::opcode:
            {
                auto item = std::make_unique<CreateAccount>();
                item->Deserialize(reader);

                return item;
            }
            case Login::opcode:
            {
                auto item = std::make_unique<Login>();
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
            case CreateAccount::opcode:
            {
                CreateAccount item;
                item.Deserialize(reader);

                return item;
            }
            case Login::opcode:
            {
                Login item;
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
            case CreateAccount::opcode:
            {
                return typeid(CreateAccount);
            }
            case Login::opcode:
            {
                return typeid(Login);
            }
        }
        assert(false);
        return typeid(nullptr);
    }
}
