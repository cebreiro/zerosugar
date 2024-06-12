#include "login_sc_generated.h"

#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr::network::login::sc
{
    void CreateAccountResult::Deserialize(PacketReader& reader)
    {
        account = reader.ReadString();
        password = reader.ReadString();
    }

    void CreateAccountResult::Serialize(PacketWriter& writer) const
    {
        writer.Write(account);
        writer.Write(password);
    }

    void LoginResult::Deserialize(PacketReader& reader)
    {
        account = reader.ReadString();
        password = reader.ReadString();
    }

    void LoginResult::Serialize(PacketWriter& writer) const
    {
        writer.Write(account);
        writer.Write(password);
    }

}
