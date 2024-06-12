#include "login_sc_generated.h"

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
        errorCode = reader.Read<int32_t>();
        authenticationToken = reader.ReadString();
        lobbyIp = reader.ReadString();
        lobbyPort = reader.Read<int32_t>();
    }

    void LoginResult::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(errorCode);
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
}
