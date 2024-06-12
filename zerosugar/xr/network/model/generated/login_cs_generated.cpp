#include "login_cs_generated.h"

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

}
