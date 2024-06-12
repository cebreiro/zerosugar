#include "gateway_object_generated.h"

namespace zerosugar::sl::gateway
{
    void UnknownObject::Deserialize(BufferReader& reader)
    {
        (void)reader;
    }

    void UnknownObject::Serialize(BufferWriter& writer) const
    {
        (void)writer;
    }

    void EndPoint::Deserialize(BufferReader& reader)
    {
        port = reader.Read<int32_t>();
        address = reader.ReadString();
    }

    void EndPoint::Serialize(BufferWriter& writer) const
    {
        writer.Write<int32_t>(port);
        writer.WriteString(address);
    }

    void CharacterAppearance::Deserialize(BufferReader& reader)
    {
        hatModelId = reader.Read<int32_t>();
        hairColor = reader.Read<int32_t>();
        skinColor = reader.Read<int32_t>();
        face = reader.Read<int32_t>();
        unk01 = reader.Read<int32_t>();
        unk02 = reader.Read<int32_t>();
        hairModelId = reader.Read<int32_t>();
        unk03 = reader.Read<int32_t>();
        unk04 = reader.Read<int32_t>();
        jacketItemId = reader.Read<int32_t>();
        unk05 = reader.Read<int32_t>();
        unk06 = reader.Read<int32_t>();
        glovesModelId = reader.Read<int32_t>();
        unk07 = reader.Read<int32_t>();
        unk08 = reader.Read<int32_t>();
        pantsId = reader.Read<int32_t>();
        unk09 = reader.Read<int32_t>();
        unk10 = reader.Read<int32_t>();
        shoesId = reader.Read<int32_t>();
        unk11 = reader.Read<int32_t>();
        unk12 = reader.Read<int32_t>();
        unk13 = reader.Read<int32_t>();
        unk14 = reader.Read<int32_t>();
        unk15 = reader.Read<int32_t>();
    }

    void CharacterAppearance::Serialize(BufferWriter& writer) const
    {
        writer.Write<int32_t>(hatModelId);
        writer.Write<int32_t>(hairColor);
        writer.Write<int32_t>(skinColor);
        writer.Write<int32_t>(face);
        writer.Write<int32_t>(unk01);
        writer.Write<int32_t>(unk02);
        writer.Write<int32_t>(hairModelId);
        writer.Write<int32_t>(unk03);
        writer.Write<int32_t>(unk04);
        writer.Write<int32_t>(jacketItemId);
        writer.Write<int32_t>(unk05);
        writer.Write<int32_t>(unk06);
        writer.Write<int32_t>(glovesModelId);
        writer.Write<int32_t>(unk07);
        writer.Write<int32_t>(unk08);
        writer.Write<int32_t>(pantsId);
        writer.Write<int32_t>(unk09);
        writer.Write<int32_t>(unk10);
        writer.Write<int32_t>(shoesId);
        writer.Write<int32_t>(unk11);
        writer.Write<int32_t>(unk12);
        writer.Write<int32_t>(unk13);
        writer.Write<int32_t>(unk14);
        writer.Write<int32_t>(unk15);
    }

    void CharacterInformation::Deserialize(BufferReader& reader)
    {
        reader.ReadBuffer(name.data(), std::ssize(name));
        reader.ReadBuffer(unk01.data(), std::ssize(unk01));
        reader.ReadBuffer(unk02.data(), std::ssize(unk02));
        reader.ReadBuffer(zone.data(), std::ssize(zone));
        reader.ReadBuffer(unk03.data(), std::ssize(unk03));
        reader.ReadBuffer(unk04.data(), std::ssize(unk04));
        reader.ReadBuffer(unk05.data(), std::ssize(unk05));
        reader.ReadBuffer(job.data(), std::ssize(job));
        arms = reader.Read<bool>();
        unk06 = reader.Read<int32_t>();
        slot = reader.Read<int8_t>();
        unk07 = reader.Read<int32_t>();
        appearanceIfTrueItemIdElseModelId = reader.Read<bool>();
        reader.ReadBuffer(unk08.data(), std::ssize(unk08));
        job1 = reader.Read<int32_t>();
        job2 = reader.Read<int32_t>();
        reader.ReadBuffer(unk09.data(), std::ssize(unk09));
        reader.ReadBuffer(unk10.data(), std::ssize(unk10));
        reader.ReadBuffer(unk11.data(), std::ssize(unk11));
        gender = reader.Read<int32_t>();
        characterLevel = reader.Read<int32_t>();
    }

    void CharacterInformation::Serialize(BufferWriter& writer) const
    {
        writer.WriteBuffer(name);
        writer.WriteBuffer(unk01);
        writer.WriteBuffer(unk02);
        writer.WriteBuffer(zone);
        writer.WriteBuffer(unk03);
        writer.WriteBuffer(unk04);
        writer.WriteBuffer(unk05);
        writer.WriteBuffer(job);
        writer.Write<bool>(arms);
        writer.Write<int32_t>(unk06);
        writer.Write<int8_t>(slot);
        writer.Write<int32_t>(unk07);
        writer.Write<bool>(appearanceIfTrueItemIdElseModelId);
        writer.WriteBuffer(unk08);
        writer.Write<int32_t>(job1);
        writer.Write<int32_t>(job2);
        writer.WriteBuffer(unk09);
        writer.WriteBuffer(unk10);
        writer.WriteBuffer(unk11);
        writer.Write<int32_t>(gender);
        writer.Write<int32_t>(characterLevel);
    }

    void Character::Deserialize(BufferReader& reader)
    {
        reader.Read(appearance);
        reader.Read(information);
    }

    void Character::Serialize(BufferWriter& writer) const
    {
        writer.Write(appearance);
        writer.Write(information);
    }

    void CharacterCreate::Deserialize(BufferReader& reader)
    {
        reader.ReadBuffer(name.data(), std::ssize(name));
        gender = reader.Read<int32_t>();
        str = reader.Read<int32_t>();
        dex = reader.Read<int32_t>();
        accr = reader.Read<int32_t>();
        health = reader.Read<int32_t>();
        intell = reader.Read<int32_t>();
        wis = reader.Read<int32_t>();
        will = reader.Read<int32_t>();
        elementalWater = reader.Read<int32_t>();
        elementalFire = reader.Read<int32_t>();
        elementalLightning = reader.Read<int32_t>();
        unk01 = reader.Read<int32_t>();
        hairColor = reader.Read<int32_t>();
        skinColor = reader.Read<int32_t>();
        face = reader.Read<int32_t>();
        hair = reader.Read<int32_t>();
        jacketId = reader.Read<int32_t>();
        glovesModelId = reader.Read<int32_t>();
        pantsId = reader.Read<int32_t>();
        shoesId = reader.Read<int32_t>();
        unk02 = reader.Read<int32_t>();
        unk03 = reader.Read<int32_t>();
        unk04 = reader.Read<int32_t>();
        unk05 = reader.Read<int32_t>();
        unk06 = reader.Read<int32_t>();
        unk07 = reader.Read<int32_t>();
        unk08 = reader.Read<int32_t>();
        unk09 = reader.Read<int32_t>();
        unk10 = reader.Read<int32_t>();
        unk11 = reader.Read<int32_t>();
        unk12 = reader.Read<int32_t>();
        unk13 = reader.Read<int32_t>();
        unk14 = reader.Read<int32_t>();
        unk15 = reader.Read<int32_t>();
        unk16 = reader.Read<int32_t>();
        unk17 = reader.Read<int32_t>();
        job = reader.Read<int32_t>();
    }

    void CharacterCreate::Serialize(BufferWriter& writer) const
    {
        writer.WriteBuffer(name);
        writer.Write<int32_t>(gender);
        writer.Write<int32_t>(str);
        writer.Write<int32_t>(dex);
        writer.Write<int32_t>(accr);
        writer.Write<int32_t>(health);
        writer.Write<int32_t>(intell);
        writer.Write<int32_t>(wis);
        writer.Write<int32_t>(will);
        writer.Write<int32_t>(elementalWater);
        writer.Write<int32_t>(elementalFire);
        writer.Write<int32_t>(elementalLightning);
        writer.Write<int32_t>(unk01);
        writer.Write<int32_t>(hairColor);
        writer.Write<int32_t>(skinColor);
        writer.Write<int32_t>(face);
        writer.Write<int32_t>(hair);
        writer.Write<int32_t>(jacketId);
        writer.Write<int32_t>(glovesModelId);
        writer.Write<int32_t>(pantsId);
        writer.Write<int32_t>(shoesId);
        writer.Write<int32_t>(unk02);
        writer.Write<int32_t>(unk03);
        writer.Write<int32_t>(unk04);
        writer.Write<int32_t>(unk05);
        writer.Write<int32_t>(unk06);
        writer.Write<int32_t>(unk07);
        writer.Write<int32_t>(unk08);
        writer.Write<int32_t>(unk09);
        writer.Write<int32_t>(unk10);
        writer.Write<int32_t>(unk11);
        writer.Write<int32_t>(unk12);
        writer.Write<int32_t>(unk13);
        writer.Write<int32_t>(unk14);
        writer.Write<int32_t>(unk15);
        writer.Write<int32_t>(unk16);
        writer.Write<int32_t>(unk17);
        writer.Write<int32_t>(job);
    }

}
