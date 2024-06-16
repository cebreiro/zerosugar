#include "lobby_message.h"

namespace zerosugar::xr::network::lobby
{
    void LobbyCharacter::Deserialize(BufferReader& reader)
    {
        slot = reader.Read<int32_t>();
        name = reader.ReadString();
        level = reader.Read<int32_t>();
        str = reader.Read<int32_t>();
        dex = reader.Read<int32_t>();
        intell = reader.Read<int32_t>();
        job = reader.Read<int32_t>();
        faceId = reader.Read<int32_t>();
        hairId = reader.Read<int32_t>();
        gold = reader.Read<int32_t>();
        zoneId = reader.Read<int32_t>();
        x = reader.Read<float>();
        y = reader.Read<float>();
        z = reader.Read<float>();
        armorId = reader.Read<int32_t>();
        glovesId = reader.Read<int32_t>();
        shoesId = reader.Read<int32_t>();
        weaponId = reader.Read<int32_t>();
        gender = reader.Read<int32_t>();
    }

    void LobbyCharacter::Serialize(BufferWriter& writer) const
    {
        writer.Write<int32_t>(slot);
        writer.WriteString(name);
        writer.Write<int32_t>(level);
        writer.Write<int32_t>(str);
        writer.Write<int32_t>(dex);
        writer.Write<int32_t>(intell);
        writer.Write<int32_t>(job);
        writer.Write<int32_t>(faceId);
        writer.Write<int32_t>(hairId);
        writer.Write<int32_t>(gold);
        writer.Write<int32_t>(zoneId);
        writer.Write<float>(x);
        writer.Write<float>(y);
        writer.Write<float>(z);
        writer.Write<int32_t>(armorId);
        writer.Write<int32_t>(glovesId);
        writer.Write<int32_t>(shoesId);
        writer.Write<int32_t>(weaponId);
        writer.Write<int32_t>(gender);
    }

}
