#include "game_sc_message.h"

#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr::network::game::sc
{
    void Test::Deserialize(PacketReader& reader)
    {
        (void)reader;
    }

    void Test::Serialize(PacketWriter& writer) const
    {
        (void)writer;
    }

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>
    {
        const int16_t opcode = reader.Read<int16_t>();
        switch(opcode)
        {
            case Test::opcode:
            {
                auto item = std::make_unique<Test>();
                item->Deserialize(reader);

                return item;
            }
        }
        return {};
    }
}
