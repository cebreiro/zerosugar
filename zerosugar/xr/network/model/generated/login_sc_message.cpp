#include "login_sc_message.h"

#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr::network::login::sc
{
    void CreateAccountResult::Deserialize(PacketReader& reader)
    {
        success = reader.Read<bool>();
    }

    void CreateAccountResult::Serialize(PacketWriter& writer) const
    {
        writer.Write<bool>(success);
    }

    void LoginResult::Deserialize(PacketReader& reader)
    {
        success = reader.Read<bool>();
        authenticationToken = reader.ReadString();
        lobbyIp = reader.ReadString();
        lobbyPort = reader.Read<int32_t>();
    }

    void LoginResult::Serialize(PacketWriter& writer) const
    {
        writer.Write<bool>(success);
        writer.Write(authenticationToken);
        writer.Write(lobbyIp);
        writer.Write<int32_t>(lobbyPort);
    }

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>
    {
        const int16_t opcode = reader.Read<int16_t>();
        switch(opcode)
        {
            case CreateAccountResult::opcode:
            {
                auto item = std::make_unique<CreateAccountResult>();
                item->Deserialize(reader);

                return item;
            }
            case LoginResult::opcode:
            {
                auto item = std::make_unique<LoginResult>();
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
            case CreateAccountResult::opcode:
            {
                CreateAccountResult item;
                item.Deserialize(reader);

                return item;
            }
            case LoginResult::opcode:
            {
                LoginResult item;
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
            case CreateAccountResult::opcode:
            {
                return typeid(CreateAccountResult);
            }
            case LoginResult::opcode:
            {
                return typeid(LoginResult);
            }
        }
        assert(false);
        return typeid(nullptr);
    }
}
