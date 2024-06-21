#include "game_message.h"

namespace zerosugar::xr::network::game
{
    void Position::Deserialize(BufferReader& reader)
    {
        x = reader.Read<float>();
        y = reader.Read<float>();
        z = reader.Read<float>();
    }

    void Position::Serialize(BufferWriter& writer) const
    {
        writer.Write<float>(x);
        writer.Write<float>(y);
        writer.Write<float>(z);
    }

    void Rotation::Deserialize(BufferReader& reader)
    {
        yaw = reader.Read<float>();
        pitch = reader.Read<float>();
        roll = reader.Read<float>();
    }

    void Rotation::Serialize(BufferWriter& writer) const
    {
        writer.Write<float>(yaw);
        writer.Write<float>(pitch);
        writer.Write<float>(roll);
    }

    void Transform::Deserialize(BufferReader& reader)
    {
        reader.Read(position);
        reader.Read(rotation);
    }

    void Transform::Serialize(BufferWriter& writer) const
    {
        writer.Write(position);
        writer.Write(rotation);
    }

    void CharacterStat::Deserialize(BufferReader& reader)
    {
        hp = reader.Read<int32_t>();
        maxHP = reader.Read<int32_t>();
        attackMin = reader.Read<int32_t>();
        attackMax = reader.Read<int32_t>();
        attackRange = reader.Read<int32_t>();
        attackSpeed = reader.Read<int32_t>();
        defence = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<int16_t>());
        level = reader.Read<int32_t>();
        gender = reader.Read<int32_t>();
        face = reader.Read<int32_t>();
        hair = reader.Read<int32_t>();
        str = reader.Read<int32_t>();
        dex = reader.Read<int32_t>();
        intell = reader.Read<int32_t>();
        stamina = reader.Read<float>();
        staminaMax = reader.Read<float>();
    }

    void CharacterStat::Serialize(BufferWriter& writer) const
    {
        writer.Write<int32_t>(hp);
        writer.Write<int32_t>(maxHP);
        writer.Write<int32_t>(attackMin);
        writer.Write<int32_t>(attackMax);
        writer.Write<int32_t>(attackRange);
        writer.Write<int32_t>(attackSpeed);
        writer.Write<int32_t>(defence);
        writer.Write<int16_t>((int16_t)std::ssize(name) + 1);
        writer.WriteString(name);
        writer.Write<int32_t>(level);
        writer.Write<int32_t>(gender);
        writer.Write<int32_t>(face);
        writer.Write<int32_t>(hair);
        writer.Write<int32_t>(str);
        writer.Write<int32_t>(dex);
        writer.Write<int32_t>(intell);
        writer.Write<float>(stamina);
        writer.Write<float>(staminaMax);
    }

    void Equipment::Deserialize(BufferReader& reader)
    {
        type = reader.Read<int32_t>();
        id = reader.Read<int32_t>();
        attack = reader.Read<int32_t>();
        defence = reader.Read<int32_t>();
        str = reader.Read<int32_t>();
        dex = reader.Read<int32_t>();
        intell = reader.Read<int32_t>();
    }

    void Equipment::Serialize(BufferWriter& writer) const
    {
        writer.Write<int32_t>(type);
        writer.Write<int32_t>(id);
        writer.Write<int32_t>(attack);
        writer.Write<int32_t>(defence);
        writer.Write<int32_t>(str);
        writer.Write<int32_t>(dex);
        writer.Write<int32_t>(intell);
    }

    void CharacterEquipment::Deserialize(BufferReader& reader)
    {
        reader.Read(armor);
        reader.Read(gloves);
        reader.Read(shoes);
        reader.Read(weapon);
    }

    void CharacterEquipment::Serialize(BufferWriter& writer) const
    {
        writer.Write(armor);
        writer.Write(gloves);
        writer.Write(shoes);
        writer.Write(weapon);
    }

    void CharacterInventoryItem::Deserialize(BufferReader& reader)
    {
        id = reader.Read<int32_t>();
        count = reader.Read<int32_t>();
        attack = reader.Read<int32_t>();
        defence = reader.Read<int32_t>();
        str = reader.Read<int32_t>();
        dex = reader.Read<int32_t>();
        intell = reader.Read<int32_t>();
    }

    void CharacterInventoryItem::Serialize(BufferWriter& writer) const
    {
        writer.Write<int32_t>(id);
        writer.Write<int32_t>(count);
        writer.Write<int32_t>(attack);
        writer.Write<int32_t>(defence);
        writer.Write<int32_t>(str);
        writer.Write<int32_t>(dex);
        writer.Write<int32_t>(intell);
    }

    void Character::Deserialize(BufferReader& reader)
    {
        instanceId = reader.Read<int64_t>();
        reader.Read(transform);
        reader.Read(stat);
        reader.Read(equipment);
        gold = reader.Read<int32_t>();
        reader.Read(items, reader.Read<int16_t>());
    }

    void Character::Serialize(BufferWriter& writer) const
    {
        writer.Write<int64_t>(instanceId);
        writer.Write(transform);
        writer.Write(stat);
        writer.Write(equipment);
        writer.Write<int32_t>(gold);
        writer.Write<int16_t>((int16_t)std::ssize(items) + 1);
        writer.Write(items);
    }

    void Monster::Deserialize(BufferReader& reader)
    {
        (void)reader;
    }

    void Monster::Serialize(BufferWriter& writer) const
    {
        (void)writer;
    }

}
